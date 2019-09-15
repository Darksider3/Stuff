import requests


class API:
    """ Interface for other APIs """
    def __init__(self, Headers = None):
        raise NotImplementedError("Subclass not NotImplemented")

    def getExistingCode(self, locatestr):
        raise NotImplementedError("Subclass should implement this")

    def runCode(self, code):
        raise NotImplementedError("Subclass should implement this")

    def putCode(self, code):
        raise NotImplementedError("Subclass should implement this")

    def getSession(self, url, headers, Session):
        response = ""
        try:
            response = Session.get(url, headers = headers)
            response.raise_for_status()
        except requests.HTTPError as err:
            print(f'HTTP Error occured: {err}')
        except Exception as err:
            print(f'Unexpected exception: {err}')
        else:
            return response # Everything worked alright
        return None # Some exception got raised!

    def postSessions(self, url, headers, data, Session):
        try:
            response = Session.post(url, headers = headers, data = data)
            response.raise_for_status()
        except requests.HTTPError as err:
            print(f'HTTP Error occured: {err}')
        except Exception as err:
            print(f'Unexpected exception: {err}')
        else:
            return response # Success!
        return None # Error

    def __unidiff(self, expected, actual, fromfile = "beforeTmp", tofile="afterTmp"):
        import difflib
        result = list(difflib.unified_diff(
            expected.splitlines(keepends=True),
            actual.splitlines(keepends=True),
            fromfile = fromfile,
            tofile = tofile
            ))
        return result
