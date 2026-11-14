R='                    if (flat.Dot(front) < 0.0f) {\n'
V=[
('gt',[(R,'                    if (0.0f > flat.Dot(front)) {\n')]),
('notge',[(R,'                    if (!(flat.Dot(front) >= 0.0f)) {\n')]),
]
