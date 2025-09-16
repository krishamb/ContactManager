#!/usr/bin/env python3
"""
String Reversal Utility
Demonstrates multiple methods to reverse strings, including long strings.
"""

import time
from typing import Callable


def reverse_string_slice(s: str) -> str:
    """
    Reverse a string using Python's slice notation.
    This is the most Pythonic and efficient method.
    
    Time Complexity: O(n)
    Space Complexity: O(n)
    """
    return s[::-1]


def reverse_string_reversed(s: str) -> str:
    """
    Reverse a string using the built-in reversed() function.
    
    Time Complexity: O(n)
    Space Complexity: O(n)
    """
    return ''.join(reversed(s))


def reverse_string_loop(s: str) -> str:
    """
    Reverse a string using a manual loop.
    Less efficient but demonstrates the algorithm.
    
    Time Complexity: O(n)
    Space Complexity: O(n)
    """
    result = []
    for char in s:
        result.insert(0, char)
    return ''.join(result)


def reverse_string_recursive(s: str) -> str:
    """
    Reverse a string using recursion.
    Note: Not recommended for very long strings due to stack overflow risk.
    
    Time Complexity: O(n)
    Space Complexity: O(n) - uses call stack
    """
    if len(s) <= 1:
        return s
    return s[-1] + reverse_string_recursive(s[:-1])


def reverse_string_two_pointer(s: str) -> str:
    """
    Reverse a string using two-pointer technique.
    Converts to list first since strings are immutable in Python.
    
    Time Complexity: O(n)
    Space Complexity: O(n)
    """
    chars = list(s)
    left, right = 0, len(chars) - 1
    
    while left < right:
        chars[left], chars[right] = chars[right], chars[left]
        left += 1
        right -= 1
    
    return ''.join(chars)


def benchmark_method(method: Callable, s: str, method_name: str) -> float:
    """
    Benchmark a string reversal method.
    
    Returns the execution time in seconds.
    """
    start_time = time.time()
    result = method(s)
    end_time = time.time()
    
    execution_time = end_time - start_time
    print(f"{method_name:25} - Time: {execution_time:.6f}s")
    
    return execution_time


def main():
    """
    Main function to demonstrate string reversal methods.
    """
    print("=" * 60)
    print("STRING REVERSAL DEMONSTRATION")
    print("=" * 60)
    
    # Test with a short string
    test_string = "Hello, World!"
    print(f"\nOriginal string: '{test_string}'")
    print(f"Reversed string: '{reverse_string_slice(test_string)}'")
    
    # Test with a longer string
    print("\n" + "-" * 60)
    print("Testing with a long string (1 million characters)")
    print("-" * 60)
    
    # Generate a long string for performance testing
    long_string = "abcdefghijklmnopqrstuvwxyz" * 40000  # ~1 million chars
    print(f"String length: {len(long_string):,} characters")
    
    # Benchmark different methods
    print("\nBenchmarking different reversal methods:")
    print("-" * 40)
    
    methods = [
        (reverse_string_slice, "Slice notation [::-1]"),
        (reverse_string_reversed, "reversed() function"),
        (reverse_string_two_pointer, "Two-pointer technique"),
        # Note: Skipping recursive and loop methods for very long strings
        # as they are significantly slower or may cause stack overflow
    ]
    
    results = {}
    for method, name in methods:
        time_taken = benchmark_method(method, long_string, name)
        results[name] = time_taken
    
    # Find the fastest method
    fastest_method = min(results, key=results.get)
    print("\n" + "=" * 40)
    print(f"Fastest method: {fastest_method}")
    print("=" * 40)
    
    # Test with Unicode and special characters
    print("\n" + "-" * 60)
    print("Testing with Unicode and special characters")
    print("-" * 60)
    
    unicode_string = "Hello 世界! 🌍 Python 🐍"
    print(f"Original: '{unicode_string}'")
    print(f"Reversed: '{reverse_string_slice(unicode_string)}'")
    
    # Test with palindrome
    print("\n" + "-" * 60)
    print("Palindrome check")
    print("-" * 60)
    
    palindrome_test = "A man a plan a canal Panama"
    cleaned = palindrome_test.replace(" ", "").lower()
    is_palindrome = cleaned == reverse_string_slice(cleaned)
    print(f"String: '{palindrome_test}'")
    print(f"Is palindrome (ignoring spaces/case): {is_palindrome}")
    
    # Memory-efficient reversal for very large strings
    print("\n" + "-" * 60)
    print("Memory-Efficient Reversal (using generator)")
    print("-" * 60)
    
    def reverse_string_generator(s: str):
        """Generator that yields characters in reverse order."""
        for i in range(len(s) - 1, -1, -1):
            yield s[i]
    
    # Example usage with generator
    sample = "Memory efficient"
    print(f"Original: '{sample}'")
    print(f"Reversed: '{''.join(reverse_string_generator(sample))}'")
    
    print("\n" + "=" * 60)
    print("SUMMARY")
    print("=" * 60)
    print("""
Recommendations:
1. For most cases: Use slice notation (s[::-1]) - fastest and most Pythonic
2. For readability: Use reversed() function with join()
3. For learning: Two-pointer technique shows the algorithm clearly
4. For very large strings: Consider using generators for memory efficiency
5. Avoid: Recursive approach for very long strings (stack overflow risk)
    """)


class StringReverser:
    """
    A class-based approach for string reversal with additional features.
    """
    
    def __init__(self, string: str):
        """Initialize with a string to reverse."""
        self.original = string
        self._reversed = None
    
    @property
    def reversed(self) -> str:
        """Lazy evaluation of reversed string."""
        if self._reversed is None:
            self._reversed = self.original[::-1]
        return self._reversed
    
    def reverse_words(self) -> str:
        """Reverse the order of words while keeping word characters in order."""
        words = self.original.split()
        return ' '.join(reversed(words))
    
    def reverse_each_word(self) -> str:
        """Reverse each word individually while maintaining word order."""
        words = self.original.split()
        reversed_words = [word[::-1] for word in words]
        return ' '.join(reversed_words)
    
    def __str__(self):
        return f"Original: '{self.original}' | Reversed: '{self.reversed}'"


def demonstrate_class_usage():
    """Demonstrate the StringReverser class."""
    print("\n" + "=" * 60)
    print("CLASS-BASED STRING REVERSER")
    print("=" * 60)
    
    text = "The quick brown fox jumps over the lazy dog"
    reverser = StringReverser(text)
    
    print(f"Original text: '{reverser.original}'")
    print(f"Fully reversed: '{reverser.reversed}'")
    print(f"Words reversed: '{reverser.reverse_words()}'")
    print(f"Each word reversed: '{reverser.reverse_each_word()}'")


if __name__ == "__main__":
    main()
    demonstrate_class_usage()