"""Conservative symbol-based inventory of executable bytes without source credit.

File-scoped local symbols are direct evidence. Global functions may be grouped
between compiler markers in *adjacent* file records, explicitly as an inference.
Other functions keep unknown file ownership. This is not a linker split config.
"""
from collections import Counter


def executable_sections(elf):
    return [{"name": s["name"], "address": hex(s["address"]), "size": s["size"]}
            for s in sorted(elf.sections, key=lambda s: s["address"])
            if s["flags"] & 6 == 6 and s["size"]]


def bounds(item):
    start = int(item["address"], 16)
    return start, start + item["size"]


def cluster_functions(functions):
    """Union overlapping entry points, preserving every symbol as evidence."""
    clusters = []
    for function in sorted(functions, key=lambda f: (f["section"], f["address"], -f["size"], f["name"])):
        start, end = function["address"], function["address"] + function["size"]
        if clusters and clusters[-1]["section"] == function["section"] and start < clusters[-1]["end"]:
            clusters[-1]["end"] = max(clusters[-1]["end"], end)
            clusters[-1]["symbols"].append(function)
        else:
            clusters.append(dict(section=function["section"], start=start, end=end, symbols=[function]))
    return clusters


def build_code_map(elf, accepted, file_owners=None):
    """file_owners: optional {(name, address): (file ordinal, evidence)} from file_map.py."""
    file_owners = file_owners or {}
    sections = {s["index"]: s for s in elf.sections if s["flags"] & 6 == 6 and s["size"]}
    files, functions = [], []
    current_file = None
    for symbol in elf.symbols():
        if symbol["type"] == 4:
            # Retain basenames, not historical developers' workstation paths.
            current_file = {"ordinal": len(files), "name": symbol["name"].replace("\\", "/").rsplit("/", 1)[-1],
                            "markers": [], "locals": []}
            files.append(current_file)
        elif symbol["section"] in sections:
            if current_file is not None and symbol["binding"] == 0 and symbol["name"] == "gcc2_compiled.":
                current_file["markers"].append(symbol)
            if symbol["type"] != 2 or symbol["size"] <= 0:
                continue
            section = sections[symbol["section"]]
            if not (section["address"] <= symbol["address"] < symbol["address"] + symbol["size"]
                    <= section["address"] + section["size"]):
                raise ValueError("Function symbol extends beyond executable section")
            owner = current_file["ordinal"] if current_file is not None and symbol["binding"] == 0 else None
            function = dict(symbol, owner=owner)
            functions.append(function)
            if owner is not None:
                current_file["locals"].append(function)

    candidates = []
    for file, following in zip(files, files[1:]):
        # Do not skip intervening files with missing markers. Do not extend the
        # last marker to section end: neither operation establishes ownership.
        if len(file["markers"]) != 1 or len(following["markers"]) != 1:
            continue
        start, end = file["markers"][0], following["markers"][0]
        if start["section"] != end["section"] or start["address"] >= end["address"]:
            continue
        a, b = start["address"], end["address"]
        section = sections[start["section"]]
        if not section["address"] <= a < b <= section["address"] + section["size"]:
            continue
        if any(not (a <= f["address"] and f["address"] + f["size"] <= b) for f in file["locals"]):
            continue
        if any(f["owner"] is not None and f["owner"] != file["ordinal"] and
               f["section"] == start["section"] and f["address"] < b and a < f["address"] + f["size"]
               for f in functions):
            continue
        candidates.append((a, b, start["section"], file["ordinal"]))
    # Out-of-order file records can produce overlapping candidate intervals.
    # Neither claimant gets to infer global ownership in that case.
    intervals = [r for r in candidates if not any(
        r != other and r[2] == other[2] and r[0] < other[1] and other[0] < r[1]
        for other in candidates)]

    clusters = cluster_functions(functions)
    groups, file_groups = [], {}
    duplicates = Counter(f["name"] for f in files)
    previous_unknown = None
    for cluster in clusters:
        start, end = cluster["start"], cluster["end"]
        intersections = [(max(start, a), min(end, b)) for a, b in accepted if a < end and start < b]
        if intersections:
            if sum(b - a for a, b in intersections) != end - start:
                raise ValueError("Accepted source cuts through a symbol overlap group")
            previous_unknown = None
            continue
        symbols = cluster["symbols"]
        item = {"address": hex(start), "size": end - start}
        if len(symbols) > 1:
            item.update(kind="overlap", name=f"Shared code: {symbols[0]['name']} … {symbols[-1]['name']}",
                        evidence="overlapping-function-symbols",
                        symbols=[{"name": s["name"], "address": hex(s["address"]), "size": s["size"]} for s in symbols])
            groups.append({"name": f"Shared entry points/{symbols[0]['name']} @ {start:08x}", "ranges": [item]})
            previous_unknown = None
            continue
        function = symbols[0]
        owner = function["owner"]
        evidence = "local-file-symbol"
        if owner is None and (function["name"], start) in file_owners:
            owner, evidence = file_owners[(function["name"], start)]
        if owner is None:
            matches = [r for r in intervals if r[2] == cluster["section"] and r[0] <= start and end <= r[1]]
            owner = matches[0][3] if len(matches) == 1 else None
            evidence = "adjacent-compiler-markers" if owner is not None else "unknown-file"
        item.update(kind="function", name=function["name"], evidence=evidence)
        if owner is not None:
            if owner not in file_groups:
                file = files[owner]
                name = file["name"] + (f" [file record {owner}]" if duplicates[file["name"]] > 1 else "")
                group = {"name": f"Unmatched files/{name}", "file_record": owner,
                         "file_name": file["name"], "ranges": []}
                if any(r[3] == owner for r in intervals):
                    a, b, _, _ = next(r for r in intervals if r[3] == owner)
                    group["marker_interval"] = [hex(a), hex(b)]
                file_groups[owner] = group
                groups.append(group)
            file_groups[owner]["ranges"].append(item)
            previous_unknown = None
        else:
            if previous_unknown is None or previous_unknown[0] != cluster["section"] or previous_unknown[1] != start:
                group = {"name": f"Unknown file/{sections[cluster['section']]['name']} @ {start:08x}", "ranges": []}
                groups.append(group)
            else:
                group = previous_unknown[2]
            group["ranges"].append(item)
            previous_unknown = (cluster["section"], end, group)

    for group in file_groups.values():
        evidence = {r["evidence"] for r in group["ranges"]}
        inferred = any(e == "adjacent-compiler-markers" or "inferred" in e for e in evidence)
        group["name"] += (" [inferred file group]" if inferred else " [local symbols only]"
                          if evidence == {"local-file-symbol"} else " [symbol evidence]")

    for index, section in sections.items():
        cursor, stop = section["address"], section["address"] + section["size"]
        gaps = []
        # Accepted source may cover code no sized symbol names (an assembly
        # file's size-0 labels), so it bounds the unidentified gaps too.
        spans = sorted([(c["start"], c["end"]) for c in clusters if c["section"] == index]
                       + [(a, b) for a, b in accepted if cursor <= a < stop])
        for start, end in spans:
            if cursor < start:
                gaps.append((cursor, start))
            cursor = max(cursor, end)
        if cursor < stop:
            gaps.append((cursor, stop))
        if gaps:
            groups.append({"name": f"Unidentified code or padding/{section['name']}",
                           "ranges": [{"kind": "unidentified", "name": f"Unidentified bytes @ {a:08x}",
                                       "address": hex(a), "size": b - a, "evidence": "no-sized-function-symbol"}
                                      for a, b in gaps]})
    result = {"schema_version": 1, "sections": executable_sections(elf), "units": groups}
    validate_code_map(result, accepted, result["sections"])
    return result


def validate_code_map(code_map, accepted, expected_sections):
    """Verify that accepted and unfinished ranges partition every code byte."""
    if code_map["schema_version"] != 1 or code_map["sections"] != expected_sections:
        raise ValueError("Code map differs from pinned executable sections")
    names = [u["name"] for u in code_map["units"]]
    if len(set(names)) != len(names) or any(not name for name in names):
        raise ValueError("Duplicate or empty mapped unit name")
    ranges = list(accepted)
    for unit in code_map["units"]:
        if not unit["ranges"]:
            raise ValueError("Empty mapped unit")
        for item in unit["ranges"]:
            start, end = bounds(item)
            if start >= end or item["kind"] not in ("function", "overlap", "unidentified"):
                raise ValueError("Invalid mapped range")
            if not item["name"]:
                raise ValueError("Unnamed mapped range")
            if item["kind"] == "overlap":
                symbols = sorted(bounds(s) for s in item["symbols"])
                if len(symbols) < 2 or symbols[0][0] != start:
                    raise ValueError("Invalid shared entry-point coverage")
                cursor = start
                for a, b in symbols:
                    if a > cursor or a < start or b <= a or b > end:
                        raise ValueError("Invalid shared entry-point coverage")
                    cursor = max(cursor, b)
                if cursor != end:
                    raise ValueError("Invalid shared entry-point coverage")
            ranges.append((start, end))
    ranges.sort()
    position = 0
    for section in expected_sections:
        cursor, stop = bounds(section)
        while position < len(ranges) and ranges[position][0] < stop:
            start, end = ranges[position]
            if start != cursor or end <= start or end > stop:
                raise ValueError("Code map coverage has a gap, overlap, or out-of-section range")
            cursor = end
            position += 1
        if cursor != stop:
            raise ValueError("Code map coverage has a gap")
    if position != len(ranges):
        raise ValueError("Code map has out-of-section ranges")
