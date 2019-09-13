import requests


class API:
    """ Interface for other APIs """
    def __init__(self, Headers = None):
        if customurl != None:
            self.Base = customurl
        
        if Headers != None:
            self.Headers = Headers

        self.Base = NotImplementedError("Subclass not NotImplemented")
        self.Session =  requests.Session()

    def getExistingCode(self, locatestr):
        raise NotImplementedError("Subclass should implement this")

    def runCode(self, code):
        raise NotImplementedError("Subclass should implement this")

    def putCode(self, code):
        raise NotImplementedError("Subclass should implement this")

    def __getSession(self, url, headers):
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

    def __postSessions(self, url, headers, data):
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
