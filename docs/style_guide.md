<!-- Taken from https://www.geeksforgeeks.org/naming-convention-in-c/ -->
<!-- With further modifications -->

# Rules
* The class name should be a noun.

* Use upper case letters as word separators, lower case for the rest of the word
  in the class name.

* The first character in the class name must be upper case, PascalCase style.

* No underscores (‘_’) are permitted in the class name.

* The private attribute name in class should be prepended with the characters
  ‘m_’.

* In a class, place attributes after methods. Place static elements before
  non-static ones `extern` as well as `virtual` should be declared first. Separate groups of methods or attributes, group by type.

* When grouping by type, order groups however you see fit, try to keep the same
  ordering between source/header.

* Source files should contain only one #include clause.

* Header files should separate include clauses grouping ones from standard
  library and external dependencies. Standard library should come first.

* Angle brackets are used for not our own includes, quotes however are for our
  own files.

* Try to include __only__ header files.

* When declaring a method/function in header file do not strip the declaration
  of variable names. This is to help with IDE autosuggestion.

* After prepending ‘m_’, rest of the name should be snake_case.

* Prefix ‘m_’ also precedes other name modifiers. For example, ‘p_’ for
  pointers.

* Each method / function name should begin with a verb, then same rules like for
  variable names should be followed. This rule does not apply for constructors
  and destructors for obvious reasons.

* Arguments should be prepended with ‘t_’, rest of the name should follow
  snake_case. Remaining prefix adding rules add their corresponding part after `t`

* Optional arguments should be prepended with ‘o_’ rest of the name should
  follow snake_case.

* The variable name should begin with an alphabet ASCII character, rest should
  follow snake_case rule.

* Digits may be used in the variable name but only after letter.

* No special symbols can be used in variable names except for the
  underscore(‘_’).

* No keywords can be used for variable names.

* Pointer variables should be prepended with ‘p_’ and place the asterisk ‘*’
  close to the variable name instead of pointer type. This also applies for
  smart pointers.

* Reference variables should be prepended with ‘r_’. This helps to differentiate
  between the method returning a modifiable object and the same method returning
  a non-modifiable object.

* Static variables should be prepended with ‘s_’.

* The global constants should be all capital ASCII letters separated with ‘_’.

* No special character is allowed in the file name except for underscore (‘_’)
  or dash (‘-‘).

* Avoid initializing multiple variables in one line to avoid common problems with initial value or pointer types

* The file name should end with the .cc (or .h for headers) extension in the
  end or should end with the .cpp extension (.hpp for headers).

* When separating template code into header and source, you can create
  implementation in .ipp file if it is necessary, however setups like these
  can get ugly with include directories in build systems.

* Do not use filenames that already exist in /user/include, or any predefined
  standard header file name.

# Illustrations
* Class
    Name a class in C++ after what it is and use upper case letters as word
    separators. The class name should be a noun. For example, ReverseString,
    Multiply2Numbers. The name should be specific and hint at the functioning of
    the class without glancing at the body of the class.

* Methods
    Every method and function performs an action, so the function name
    should make it clear what it does. For example, WriteDataToFile() is more
    convenient name than WriteFile().

* Constants
    Constants should be all capital letters with ‘_’ separators. For example,
    MAX_INT, TWO_PI, etc.

# Examples
* Classes and class attribute names
    * The class name should be a noun.
    * Use upper case letters as word separators, lower case for the rest of the
      word.
    * The first character in the class name must be upper case.
    * No underscores (‘_’) are permitted in the class name.

    ```cpp
    class Rectangle
    {
    public:
      int area;
      int perimeter;

    private:
      int m_length;
      int m_width;
    };
    ```
* Class elements ordering
  * In a class, place attributes after methods. Place static elements before
    non-static ones. Separate groups of methods or attributes, group by type.

    ```cpp
    class DataProcessor
    {
    public:
      DataProcessor();

      void process_data();
      void package_results();

      int last_status;
    private:
      static constexpr std::size_t S_MAX_DATA_CHUNK_SIZE{ 64000 };

      bool validate_parsed_data();

      int m_error_code;
    };
    ```

* Functions and function argument names
    Usually, every function in C++ performs one or more actions, so the name of
    the function should clearly hint what it does. Each method / function name
    should begin with a verb.

    * Prefixes are sometimes useful. For example,
      * get_ get value.
      * set_ set value.

    The same name convention is used as that for the variable/attribute names.
    ```cpp
    int get_value();
    int solve_equation();
    ```

    Arguments should be prepended with ‘t_’, rest of the name should follow snake_case.

    ```cpp
    int connect_to_database(int t_access_mode, std::string t_database_address);
    ```

    Optional arguments should be prepended with ‘o_’ rest of the name should follow
    snake_case.

    ```cpp
    int open_file(std::string t_filename, std::string to_mode);
    ```

* Variables
    When the variables are declared dynamically using the new keyword or if the variables are declared as class attributes then they take memory from the heap and when the variables are created in a C++ program, the memory is allocated from the program stack.

    * The variable name should begin with an alphabet ASCII character, rest
      should follow snake_case rule.
    * Digits may be used in the variable name but only after letter.
    * No special symbols can be used in variable names except for the
      underscore(‘_’).
    * No keywords can be used for variable names.

    ```cpp
    int total_cost;
    int length;
    ```

    Pointer variables should be prepended with ‘p_’ and place the asterisk ‘*’
    close to the variable name instead of pointer type. This also applies for
    smart pointers.

    ```cpp
    int *p_name;
    int *p_age, address; // Here only p_age is a pointer variable !
    std::unique_ptr<double> p_length;
    ```

    Reference variables should be prepended with ‘r_’. This helps to
    differentiate between the method returning a modifiable object and the same
    method returning a non-modifiable object.

    ```cpp
    int weight = 8;
    int& r_weight = weight;
    ```

    Static variables should be prepended with ‘s_’.
    ```cpp
    static int s_count;
    ```
    
    Avoid initializing multiple variables in one line to avoid common problems 
    with initial value or pointer types.
    ```cpp
    int* a, b, c; // only a is a pointer !
    int a, b, c = 0; // only c is initialized !
    ```

* Constants
    The global constants should be all capital letters separated with ‘_’.
    ```cpp
    const double TWO_PI = 6.28318531;
    ```

* Includes
  * Source files should contain only one include clause.
  * Header files should separate include clauses grouping ones from standard
    library and external dependencies. Standard library should come first.
  * Angle brackets are used for not our own includes, quotes however are for our
    own files.
  * Try to include __only__ header files.

    ```cpp
    // file: my_header.hpp
    #ifndef MY_HEADER_HPP
    #define MY_HEADER_HPP

    #pragma once

    #include <iostream>
    #include <vector>

    #include <fmt/core.h>

    /* code */

    #endif//MY_HEADER_HPP
    ```

    ```cpp
    // file: my_source.cpp
    #include "my_header.hpp"

    /* code */
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