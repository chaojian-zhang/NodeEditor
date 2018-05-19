#pragma once
// Hash only, no table storage
// Internally treat everything as ANSI
class Hash
{
public:
	static unsigned int HashID(const unsigned char* string, unsigned int hashRange);

private:
};

// References: The Purpose Is To Write a Clean Function Rather Than Use A Library Which Hides Everything
// And We Care Only Practical Usage, Not Academical Value
/* General Introduction <Not Practically Conceptually Useful Since No Implementation Detail Is Mentioned>
https://en.wikipedia.org/wiki/Cyclic_redundancy_check
https://en.wikipedia.org/wiki/Hash_table
https://en.wikipedia.org/wiki/Birthday_problem
https://en.wikipedia.org/wiki/Birthday_attack

Technical Introduction and Implementation <Not Practically Useful Since Too F*** Long To Read>
https://en.wikipedia.org/wiki/Computation_of_cyclic_redundancy_checks
http://www.ti.com/lit/an/spra530/spra530.pdf

Implementation Examples <We do not care about the theory and statistical measurement as long as it works, that why those codes are muc hmore
easier to read and to use>
http://www.cse.yorku.ca/~oz/hash.html

Discussions and Implementations
http://stackoverflow.com/questions/8317508/hash-function-for-a-string
http://stackoverflow.com/questions/6072174/how-large-should-a-hashtable-be-initialized-related-to-the-entries-count
http://stackoverflow.com/questions/628790/have-a-good-hash-function-for-a-c-hash-table

- People are saying that CRC32 works best, but since it is too troublesome to find some practical reference, we will stick with djb2 for now
*/