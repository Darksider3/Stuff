import requests
import sys, html
from bs4 import BeautifulSoup

#curl 'https://rextester.com/TSBV14831' -H 'User-Agent: Mozilla/5.0 (X11; Linux x86_64; rv:70.0) Gecko/20100101 Firefox/70.0' -H 'Accept: text/html,application/xhtml+xml,application/xml;q=0.9,*/*;q=0.8' -H 'Accept-Language: de' --compressed -H 'Referer: https://rextester.com/l/cpp_online_compiler_clang' -H 'Connection: keep-alive' -H 'Cookie: ASP.NET_SessionId=t4iaj3ycanhikkvb5aepiuq2' -H 'Upgrade-Insecure-Requests: 1' -H 'Cache-Control: max-age=0'
Headers = {"User-Agent": "Mozilla/5.0 (X11; Linux x86_64; rv:7    0.0) Gecko/20100101 Firefox/70.0",
        "Referer": "https://rextester.com/l/cpp_online_compiler_clang", "charset": "UTF-8"}
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

#curl 'https://rextester.com/rundotnet/Run' -H 'User-Agent: Mozilla/5.0 (X11; Linux x86_64; rv:70.0) Gecko/20100101 Firefox/70.0' -H 'Accept: text/plain, */*; q=0.01' -H 'Accept-Language: de' --compressed -H 'Content-Type: application/x-www-form-urlencoded; charset=UTF-8' -H 'X-Requested-With: XMLHttpRequest' -H 'Origin: https://rextester.com' -H 'Connection: keep-alive' -H 'Referer: https://rextester.com/l/cpp_online_compiler_clang' --data 'LanguageChoiceWrapper=27&EditorChoiceWrapper=1&LayoutChoiceWrapper=1&Program=%23include+%3Ciostream%3E%0D%0Aint+main()%0D%0A%7B%0D%0A++++std%3A%3Acout+%3C%3C+%22Hello%2C+world!%5Cn%22%3B%0D%0A%7D&CompilerArgs=-Wall+-std%3Dc%2B%2B14+-stdlib%3Dlibc%2B%2B+-O2+-o+a.out+source_file.cpp&Input=&ShowWarnings=false&Privacy=&PrivacyUsers=&Title=&SavedOutput=&WholeError=&WholeWarning=&StatsToSave=&CodeGuid=&IsInEditMode=False&IsLive=False'
def runCode(code: str, lang: int) -> dict:
    url = f'{Base}rundotnet/Run'
    H = Headers
    H["Content-Type"] = "application/x-www-form-urlencoded"
    H["X-Requested-With"] = "XMLHttpRequest"
    H["Origin"] = "https://rextester.com"
    import urllib
    data = {"LanguageChoiceWrapper": 27, "EditorChoiceWrapper":1, "LayoutChoiceWrapper":1, "Program": code, 
            "CompilerArgs": "-Wall -std=c++14 -stdlib=libc++ -O2 -o a.out source_file.cpp"}
    data = urllib.parse.urlencode(data)

    try:
        response = Session.post(url, data=data, headers=Headers)
        response.raise_for_status()
    except requests.HTTPError as err:
        print(f"Couldn't let that code run! :/ HTTP-Error: {err}")
    except Exception as err:
        print(f"Uncatchable exception occured: {err}")
    else:
        return response.text;
    return
    
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

TestCode = """#include <iostream>
int main()
{
    std::cout << "Hello, world!\\n";
}
"""
print(runCode(TestCode, 27))
