NAFT Format Specification
=========================

NAFT: Node-Attribute-Freetext-Tree

## Requirements

1. Easy to write (both human and machine)
2. Easy to read/parse (both human and machine)
3. Format can be used to express object hierarchies
  * Efficient expression of types, _key_/_value_-based attributes and nesting
4. Every message string is a valid naft document
  * No restriction on _tags_, _keys_ and _values_
  * NAFT pieces can be embedded in a text document with reasonable restrictions on the text
5. Streaming support
  * Data can already be extracted without need to have the complete message string
6. Well-defined transformation from serialized to deserialized, and back
  * This allows embedding active elements into a document that can be parsed, executed and rendered again
7. Binary variant for low-bitrate serial transmission
  * Easy to parse on low-end embedded systems
8. Low probability of interfering with existing formats
  * XML, [ANSI escape codes](https://en.wikipedia.org/wiki/ANSI_escape_code#Escape_sequences), JSON

## Tags

A _tag_ is communicated by enclosing it with `[]`: `[tag]`.

The _tag_ itself can contain `[` or `]` characters. After finding the opening `[` character, the parser will look for the closing `]` character to identify the _tag_. A `]` character can only be a closing `]` character if it saw the same amount of `[` and `]` characters. If the `[]` characters happen to be out-of-balance, you can balance them while instructing the parser to _not include_ those in the actual _tag_ by prepending it with the `^` character. `^` characters are used as-is, when they happen to appear in front of a `[` on `]` character, they should be duplicated.

This system ensures an easy parser implementation: once the opening character '[' for a tag is found, it needs to collect all the characters until the matching ']' character is found. Each additional '[' character requires an additional ']' character to be found before the tag is completely parsed. Once the raw tag is found, it should be processed for the '^' escape characters.

* `abc` => `[abc]`
* `ab[]c` => `[ab[]c]`
* `ab[c` => `[ab[^]c]` or `[ab[c^]]`
* `ab]c` => `[ab^[]c]` or `[^[ab]c]`
* `ab^c` => `[ab^c]`
* `ab^[c` => `[ab^^[^]c]`

## Attributes

Attributes are wrapped in `()` and have the same balanced parsing system as _tags_, but now the balanced requirement is on the `()` characters. Attributes should come _directly_ after the _tag_. If characters are found after a _tag_ or _attribute_ (other than whitespace characters), the _tag_ and its _attributes_ are complete, a parser can signal it found a new tag with attributes.

Next to this, _attributes_ consist of both a _key_ and a _value_, separated by the first `:` character found, if any. If none is present, the _value_ is assumed to be empty.

If the _key_ itself contains a `:` character, this has to be signalled by prepending it with the `^` character. The _value_ can contain `:` characters, they should not be prepended with a `^` character.

* `[tag](a:0)(b:1)(a:2)`: attribute a will be overwritten with 2
* `[tag](a)`: only key "a" is present, its value is empty
* `[tag](:a)`: key is empty (""), value is "a"
* `[tag](a:b:c)`: key is "a", value is "b:c"
* `[tag](a^:b:c)`: key is "a:b", value is "c"

## Nesting

After the attributes, _tags_ can be nested, the nesting scope is expressed via `{}`.

* `[a][b]`: the _a_ and _b_ tags are at the same level
  * `a`
  * `b`
* `[a]{[b]}[c]`: _b_ is nested under `a`, `c` is at the same level of `a` again
  * `a`
    * `b`
  * `c`

## White space

White space characters (space, tab, newline and carriage return) are ignored when placed in-between _tags_ and _attributes_. Other characters that cannot be interpreted as part of the naft format are ignored, but can be used to close the attribute list.

* `[tag]`: parser cannot know if attributes will be present yes or no, and cannot signal this yet
* `[tag].`: parser knows no attributes will be given, and can signal this. The `.` is not signalled.
* `[tag].(a:0)`: the extra "(a:0)" is ignored.
* `[tag](a:0).`: parser knows only one attribute was given, and can signal this. The `.` is not signalled.
