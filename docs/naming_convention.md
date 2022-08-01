<!-- Taken from https://www.geeksforgeeks.org/naming-convention-in-c/ -->
<!-- With further modifications -->

# Rules
* The class name should be a noun.

* Use upper case letters as word separators, lower case for the rest of the word
  in the class name.

* The first character in the class name must be upper case.

* No underscores (‘_’) are permitted in the class name.

* The private attribute name in class should be prepended with the character
  ‘m’.

* After prepending ‘m’, the same rules will be followed for the name like that
  for the class name.

* Character ‘m’ also precedes other name modifiers also. For example, ‘p’ for
  pointers.

* Each method / function name should begin with a verb.

* The first character of function / method argument names should be lower case.
  All words starting after the first letter should be the upper case with class
  names.

* The variable name should begin with an alphabet.

* Digits may be used in the variable name but only after the alphabet.

* No special symbols can be used in variable names except for the
  underscore(‘_’).

* No keywords can be used for variable names.

* Pointer variables should be prepended with ‘p’ and place the asterisk ‘*’
  close to the variable name instead of pointer type.

* Reference variables should be prepended with ‘r’. This helps to differentiate
  between the method returning a modifiable object and the same method returning
  a non-modifiable object.

* Static variables should be prepended with ‘s’.

* The global constants should be all capital letters separated with ‘_’.

* No special character is allowed in the file name except for underscore (‘_’)
  and dash (‘-‘).

* The file name should end with the .cc (or .h for headers) extension in the
  end or should end with the .cpp extension (.hpp for headers).

* When separating template class into header and source, you can create
  implementation in .ipp file if it is necessary, however setups like these
  can get ugly with include directories in build systems.

* Do not use filenames that already exist in /user/include. or any predefined
  header file name.

# Illustrations
* Class
    Name a class in C++ after what it is and use upper case letters as word
    separators. The class name should be a noun. For example, ReverseString,
    Multiply2Numbers. The name should be specific and hint at the functioning of
    the class without glancing at the body of the class. This type of
    conventions called PascalCase.

* Methods
    Every method and function performs an action, so the function name
    should make it clear what it does. For example, writeDataToFile() is more
    convenient name than WriteFile(). Function names should begin with a verb.
    This type of conventions called camel case. We can also follow snake_case
    for methods. In snake case we have to join two separate words using ‘_’
    (Underscore).

* Constants
    Constants should be all capital letters with ‘_’ separators. For example,
    MAX_INT, TWO_PI, etc.

# Examples
* Class and Class Attributes names
    * The class name should be a noun.
    * Use upper case letters as word separators, lower case for the rest of the
      word.
    * The first character in the class name must be upper case.
    * No underscores (‘_’) are permitted in the class name.

    ```cpp
    class PerimeterRectangle
    {
    public:
        int perimeter;

    private:
        int mLength;
        int mWidth;
    };
    ```

* Functions and Function Argument names
    Usually, every function in C++ performs one or more actions, so the name of
    the function should clearly hint what it does. Each method / function name
    should begin with a verb.

    * Suffixes are sometimes useful. For example,
        * Count- the current count of the counter.
        * Key- the key value.

    * Prefixes are sometimes useful. For example,
        * get- get value.
        * set- set value.

    The same name convention is used as that for the class names.
    ```cpp
    int getValue();
    int SolveEquation();
    ```

    The first character of function / method argument names should be lower
    case. All words starting after the first letter should be the upper case
    with class names.
    ```cpp
    int PerimeterRectangle(int lengthRectangle, int widthRectangle);
    ```
    
* Variables
    When the variables are declared dynamically using the new keyword or if the variables are declared as class attributes then they take memory from the heap and when the variables are created in a C++ program, the memory is allocated from the program stack.

    * The variable name should begin with an alphabet.
    * Digits may be used but only after the alphabet.
    * No special symbols can be used in variable names except for the
      underscore(‘_’).
    * No keywords can be used for variable names.

    ```cpp
    int total_cost;
    int length;
    ```

    Pointer variables should be prepended with ‘p’ and place the asterisk ‘*’
    close to the variable name instead of pointer type.

    ```cpp
    int *pName;
    int *pAge, address; // Here only pAge is a pointer variable !
    ```

    Reference variables should be prepended with ‘r’. This helps to
    differentiate between the method returning a modifiable object and the same
    method returning a non-modifiable object.

    ```cpp
    int weight = 8;
    int& rweight = weight;
    ```

    Static variables should be prepended with ‘s’.
    ```cpp
    static int sCount;
    ```

* Constants
    The global constants should be all capital letters separated with ‘_’.
    ```cpp
    const double TWO_PI = 6.28318531;
    ```

* File naming
    * No special character is allowed in the file name except for underscore
      (‘_’) and dash (‘-‘).
    * The file name should end with the .c / .cc (or .h for headers) extension
      in the end or should end with the .cpp extension (.hpp for headers).
    * When separating template class into header and source, you can create
      implementation in .ipp file if it is necessary, however setups like these
      can get ugly with include directories in build systems.
    * Do not use filenames that already exist in /user/include. or any
      predefined header file name.

    ```
    helloworld.c        // Valid

    helloworld.cc       // Valid

    helloworld.h        // Valid

    hello_world.cpp     // Valid

    hello_world.ipp     // Valid

    hello_world.hpp     // Valid

    hello-world.cpp     // Valid

    hel-lo_world.cpp    // Valid

    hello* world.cpp    // Not Valid

    iostream.cpp        // Not Valid
    ```