from config import *
if DBG_ENABLED:
    from inspect import currentframe, getframeinfo

    def dbg(*args, **kwargs):
        frameinfo = getframeinfo(currentframe().f_back)
        ret = f"{frameinfo.filename}:{frameinfo.lineno}:" + "".join(map(str, args))
        if type(*args) == dict and DBG_PP_DICT:
            import pprint
            pp = pprint.PrettyPrinter(depth=4)
            pp.pprint(*args)
        else:
            print(ret, *kwargs)
        return

elif not DBG_ENABLED:
    def dbg(*xargs):
        pass