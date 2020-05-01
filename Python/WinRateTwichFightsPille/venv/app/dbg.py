from config import *
if DBG_ENABLED:
    from inspect import currentframe, getframeinfo
    def dbg(*args, **kwargs):
        frameinfo = getframeinfo(currentframe().f_back)
        ret = f"{frameinfo.filename}:{frameinfo.lineno}:" + "".join(map(str, args))
        print(ret, *kwargs)
        return

elif not DBG_ENABLED:
    def dbg(*xargs):
        pass