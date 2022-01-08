# hw1 report

|Field|Value|
|-:|:-|
|Name|吳泓緯|
|ID|0716060|

## How much time did you spend on this project

I spent up to 10 hours on this project.

## Project overview

The source code of this project is placed in the scanner.l file.

In order to build a scanner to pass the test cases provided by TAs, I divide all regular expression rules into 14 sections. I refer to the TAs' introduction of this project to arrange all sections.

Section:
- Space & Tab
- Pseudocomment
- Comment
- Delimiters
- Arithmetic
- Relational
- Logical
- Keywords
- Identifier
- Integer Constants
- Float-Point Constants
- Scientific Notation
- String
- Error Handler

## What is the hardest you think in this project

The hardest part I encountered is that I had tried to match all test cases of one kind of token with only one regular expression.

After numerous trial and error, I thought that I should divide a regular expression which describes one kind of token into many rules. In other words, each regular expression will be smaller range.

It will be easier to maintain and debug.

## Feedback to T.A.s

> Thank you for preparing so many test cases. With them, it is heplful for me to wirte regular expression.
