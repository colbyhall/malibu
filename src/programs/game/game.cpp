#include <core/core.hpp>

#include <cstdio>

int main(int argc, char** argv) {
	Array<usize> foo;
	while (foo.len() < 1028) {
		foo.push(foo.len());
	}

    return 0;
}