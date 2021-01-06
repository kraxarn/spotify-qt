While the spotify-qt code style is mostly inconsistent, it tries to follow
the style of Qt. spotify-qt-lib however, is trying to have a code style more
similar to the standard C++ library. It's mostly based of
[povilasb/style-guides](https://github.com/povilasb/style-guides/blob/master/cpp.rst).

### Formatting
* 80 character lines.
* Indent with tabs, 4 space width.
* Unix LF line endings (\n).
* Brackets are always on a new line.

### Namespaces
Namespaces are in all lower-case with underscore separators. Usually very short.
```c++
namespace lib
{
	// spt instead of spotify
	namespace spt
	{
			// ...
	}
}
```

### Classes
```c++
class album_track: public track
{
public:
	// explicit for constructors with a single argument
	explicit album_track(const std::string &id);

protected:
	// ...

private:
	// ...
};
```

### Enums
Same style for both `enum` and `enum class`. The last line ends with a comma.
```c++
enum class log_type
{
	// enum classes don't use any prefix
	information,
	warning,
	error,
};

enum log_type
{
	// enums use a simplified version of the name of the enum
	log_information,
	log_warning,
	log_error,
};
```

### If/else
No brackets are used when content in statement is a single line only (not 
necessarily a single statement).
```c++
if (true)
	do_something();

if (true)
{
	something_with_a_long_name()
		.do_something_with_it();
}
```

### Other stuff
```c++
try
{
	// ...
}
catch (const std::exception &e)
{
	// ...
}

switch (0)
{
	case 0:
		break;

	case 1:
		break;

	default:
		return;
}
```

## Naming

### Files
Header files use .hpp extension and source files use .cpp extension. Files 
are named after the class, enum, etc. containing it without any spaces.

`artisttrack.hpp`:
```c++
class artist_track
{
	// ...
};
```

### Fields
Fields are lower-case with underscores. Getters and/or setters are prefixed 
using `get_` or `set_`. Avoid setters having the same parameter name as the 
property itself, as that will create a naming conflict.
```c++
class track
{
public:
	void load_something();

	void set_id(const std::string &value)
	{
		// Usually defined in source file
		id = value;
	}

	std::string get_id()
	{
		return id;
	}
	
private:
	std::string id;
};
```
