# string-toolkit

You can install from pip with:
```bash
$ python3 -m pip install string-toolkit
```

### Example
```py
>>> import st
>>> st.to_abbreviation("Hello, World!")
'HW'
```

### Trying with more functions
```py
>>> st.get_indexes("Hello, World!", "l")
[2, 3, 10]
>>>
>>> st.has_custom_emoji("Hello!")
False
>>> st.has_custom_emoji("Hello, <:hello:69696969696969696>!")
True
>>>
>>> st.mock("Hello, World!")
'HeLlO, WOrLd!'
>>>
>>> st.proper_case("HeLlO, WOrLd!")
'Hello, World!'
>>>
>>> st.scramble("Hello, World!")
'd,ll relooHW!'
>>>
>>> st.shorten("Hello, World!", 5, "...")
'Hello...'
>>>
>>> st.substr("Hello, World!", 7, 5)
'World'
>>>
>>> st.progress_bar(50, 100, "=", "@", ".", 30)
'===============@..............'
>>>
>>> st.to_chunks("Hello, World!", 5)
['Hello', ', Wor', 'ld!']
```

### C/C++ header
This package is a wrapper around a C/C++ header! [see the documentation for it here.](https://github.com/vierofernando/string_toolkit.h#readme)