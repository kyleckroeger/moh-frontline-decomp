BLOCK='''    hint = GetHint(objective);
    texts = hint->text;
    nextIndex = &hint->next;
    next = hint->next;
'''
IND='    '
S=['hint = GetHint(objective);','texts = hint->text;','nextIndex = &hint->next;','next = hint->next;']
DEP={0:[],1:[0],2:[0],3:[0]}
