//this is a header file where I will keep some functions that I might need in different other files.

#pragma once
#include <string_view>
#include <cctype> //for std::tolower

//what is this 'inline'?
//normally we declare a function in .h file and define it's logic in separate .cpp file.Now when two diferent .cpp files includes our header file compiler doesn't complain cause the definition exists in exactly one place.
//but if we define a function directly in header file(like what are we doing below) and two diferent .cpp files includes our header file --> linker sees multiple definitions of same function--> compiler throws error.
//so we use inline.inline tells the compiler and linker:"This function may appear in multiple translation units. That's intentional — don't treat it as a duplicate, just use whichever copy you find."
//what actually happens is when there is inline present the compiler pastes the fucntion definition in the calling cpp code.That way the linker doesn't need to find the .so file(the machine code) for that function.

inline bool is_same(std::string_view a,std::string_view b){ //this function is a helper function to check if two string_view is same irrespective of case(upper or lower).
    //the reason we don't pass arguement by reference here cause we just need to read,no need to advance cursor like other functions are doing.
    if (a.size()!=b.size()) return false; //if size doesn't match they ain't same
    for (size_t i=0;i<a.size();i++){
        if (std::tolower(a[i])!=std::tolower(b[i])) return false; //if one character doesn't match they ain't same.
    }
    return true; //otherwise they are same
}
