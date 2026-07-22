/*okay so here is the deal.
till now I had 'config.py' file and our entry_point.py was fetching config values from it.
but there was two issues because only python can read values directly from config.py:
    1. I had to create a global variable in subsequent cpp files.then in the entry_point.py i had to pass the config value as an argument
    before assigning that value to that global variable.all this hassle just to use that config value in the cpp code.
    2. another issue is I need some config values inside cpp code that is not called from our entry_point.py.so how can we pass the config value in that cpp code?


so I decided to switch from config.py to config.env cause unlike config.py which is just a python file and can be read only by python code
config.env is just a text file.any language can read it-->then load in the process environment-->and then later get the value from process environment table.
what is process environment?
Every running program (process) has a built-in key-value store maintained by the operating system called the process environment. It's separate from any file, separate from any language.

so there are two distinct stages- 
    stage 1: we load the key=value pairs into the process environment.
    stage 2: we fetch values from that process environmnet table.
different language do these steps differently.

for example,

[python]
    stage 1
        load_dotenv("config.env")
        # reads config.env file
        # puts each KEY=VALUE into os.environ
        # which IS the process environment table
    stage 2
        thread_count=os.environ.get("MAX_THREAD_COUNT")
        # fetch value of MAX_THREAD_COUNT from the process environment table

[cpp]
    stage 1
        via setenv() it loads the KEY=VALUE pair in process environment table
    stage 2
        via std::getenv() it fetch value from the process environment table

okay so first I thought in the entry_point.py file I will use load_dotenv() to load the config.env file(to be accurate read the config.env file and load key=value pair) in the process environment table
every .so file loaded by Python inherits the same process environment. They all see the same variables. No special setup needed in any of them.
and now that the process environment table is set we can fetch those values in any files using `os.environ.get()`(if it's python) and `std::getenv()` (if it's cpp)

but that has two issues:
    1. in that scenario we have to make sure that load_dotenv() always runs before any cpp shared library(.so) is loaded using ctypes.CDLL()
    because those shared library is nothing but compiled cpp code where we will use std::getenv() to fecth values from process environmnet table.
    so if the process environment table is not loaded yet then it will crash.However it's not a critical issue cause i can just make sure that it's loaded before '.so' files.
    2. the critical issue is-during testing I might have to run the cpp files solo/individually.in that case the env file will not be loaded cause the entry_point.py is not being ran.
    and as we are trying to fetch values without loading them in process environment table our program will crash.

so I think I have to do something hybrid.I will load env file at the beginning of entry_point.py file 
but I will also load them in my individual cpp code with a parameter(0 as third argument in setenv()) to prevent overwriting in case our python code has already loaded the values in the table.
I don't know right now if I should also do something for other way around like if cpp has loaded the values later other file don't overwrite it.
*/

//in cpp we need to create a custom function to load the KEY=VALUE pair from the env file in the process environment table.cause there is no direct method like load_dotenv() in cpp.
//here I will keep those functions.inside utils cause multiple files will call this functions.