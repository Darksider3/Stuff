import requests
import sys, html
from bs4 import BeautifulSoup

#curl 'https://rextester.com/TSBV14831' -H 'User-Agent: Mozilla/5.0 (X11; Linux x86_64; rv:70.0) Gecko/20100101 Firefox/70.0' -H 'Accept: text/html,application/xhtml+xml,application/xml;q=0.9,*/*;q=0.8' -H 'Accept-Language: de' --compressed -H 'Referer: https://rextester.com/l/cpp_online_compiler_clang' -H 'Connection: keep-alive' -H 'Cookie: ASP.NET_SessionId=t4iaj3ycanhikkvb5aepiuq2' -H 'Upgrade-Insecure-Requests: 1' -H 'Cache-Control: max-age=0'

Headers = {"User-Agent": "Mozilla/5.0 (X11; Linux x86_64; rv:7    0.0) Gecko/20100101 Firefox/70.0",
        "Accept": "text/html,application/xhtml+xml,application/xm    l;q=0.9,*/*;q=0.8",
        "Referer": "https://rextester.com/l/cpp_online_compiler_clang",}

Base = "https://rextester.com/"

Session = requests.Session()

def getExistingCode(url: str, withbase=False) -> str:
    ret = ""
    if withbase:
        url = f'{Base}{url}'

    try:
        response = Session.get(url, headers=Headers)
        response.raise_for_status()
    except requests.HTTPError as http_err:
        print(f"HTTP Error: {http_err}")
    except Exception as e:
        print(f"Unexpected exception occured: {e}")
    else:
        print("Response OK")
    responseHTML = response.text;
    try:
        soup = BeautifulSoup(responseHTML, features="lxml")
        if type(soup.textarea.string) != "NoneType":
            ret = soup.textarea.string.strip() # couldnt get a freakin' exception without -.-
    except AttributeError as e:
        print(f'Couldn\'t parse textarea to obtain code; probably not even there?')
    #  <textarea class="editor" spellcheck="false" cols="1000" id="Program" name="Program" rows="30" style="width: 100%;resize:none;">$CODE</textarea>
    return ret


# uploads code and returns the new url
def putCode(code: str) -> str:
    return;

def _unidiff_output(expected, actual):
    """
    Helper function. Returns a string containing the unified diff of two multiline strings.
    """

    import difflib
    
    #expected = [line for line in expected.split('\n')]
    #actual = [line for line in actual.split('\n')]
    #diff=difflib.unified_diff(expected, actual)
    #d = difflib.Differ()
    result = list(difflib.unified_diff(expected.splitlines(keepends=True), actual.splitlines(keepends=True), fromfile="tmpBefore", tofile="tmpAfter"))
    #result = list(d.compare(expected.splitlines(keepends=True), actual.splitlines(keepends=True)))
    return result

Exist= getExistingCode("https://rextester.com/TSBV14831")
print(Exist)
from pprint import pprint
sys.stdout.writelines(_unidiff_output('std::cout << "Hello, world!\n";', Exist))
