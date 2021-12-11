# spotify-qt-lib unit testing
"Unit testing is usually good" -Someone probably

## Structure
Each class should have a matching test file.
Each test file should contain a `TEST_CASE` with the name of the class,
and then a `SUBCASE` for each method in the class with the actual tests.

```cpp
TEST_CASE(/*class*/)
{
	SUBCASE(/*method*/)
	{
		// ...
	}
}
```