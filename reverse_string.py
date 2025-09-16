#!/usr/bin/env python3
"""
String Reversal Utility

This module provides efficient methods to reverse strings, including very long ones.
"""

def reverse_string_slicing(text):
    """
    Reverse a string using Python's slicing notation.
    Most Pythonic and efficient for most use cases.
    
    Args:
        text (str): The string to reverse
        
    Returns:
        str: The reversed string
    """
    return text[::-1]


def reverse_string_iterative(text):
    """
    Reverse a string using iterative approach.
    Memory efficient for very long strings.
    
    Args:
        text (str): The string to reverse
        
    Returns:
        str: The reversed string
    """
    result = []
    for i in range(len(text) - 1, -1, -1):
        result.append(text[i])
    return ''.join(result)


def reverse_string_recursive(text):
    """
    Reverse a string using recursion.
    Note: Not recommended for very long strings due to recursion depth limits.
    
    Args:
        text (str): The string to reverse
        
    Returns:
        str: The reversed string
    """
    if len(text) <= 1:
        return text
    return text[-1] + reverse_string_recursive(text[:-1])


def reverse_string_builtin(text):
    """
    Reverse a string using built-in reversed() function.
    
    Args:
        text (str): The string to reverse
        
    Returns:
        str: The reversed string
    """
    return ''.join(reversed(text))


def reverse_string_chunks(text, chunk_size=1000000):
    """
    Reverse a very long string by processing it in chunks.
    Useful for strings that might cause memory issues.
    
    Args:
        text (str): The string to reverse
        chunk_size (int): Size of each chunk to process
        
    Returns:
        str: The reversed string
    """
    if len(text) <= chunk_size:
        return text[::-1]
    
    chunks = []
    for i in range(0, len(text), chunk_size):
        chunk = text[i:i + chunk_size]
        chunks.append(chunk[::-1])
    
    # Reverse the order of chunks and join them
    return ''.join(reversed(chunks))


def main():
    """
    Demonstration of different string reversal methods.
    """
    # Test with various string lengths
    test_strings = [
        "Hello, World!",
        "A man a plan a canal Panama",
        "12345" * 1000,  # Medium length string
        "abcdef" * 100000,  # Long string
    ]
    
    print("String Reversal Demonstration")
    print("=" * 50)
    
    for i, test_str in enumerate(test_strings, 1):
        print(f"\nTest {i}: String length = {len(test_str)}")
        
        if len(test_str) <= 50:
            print(f"Original: {test_str}")
        else:
            print(f"Original (first 25 chars): {test_str[:25]}...")
        
        # Use the most efficient method for demonstration
        reversed_str = reverse_string_slicing(test_str)
        
        if len(reversed_str) <= 50:
            print(f"Reversed: {reversed_str}")
        else:
            print(f"Reversed (first 25 chars): {reversed_str[:25]}...")
        
        # Verify the reversal is correct
        double_reversed = reverse_string_slicing(reversed_str)
        is_correct = double_reversed == test_str
        print(f"Verification: {'✓ PASS' if is_correct else '✗ FAIL'}")


if __name__ == "__main__":
    # Example usage
    print("Example: Reversing a long string")
    long_string = "This is a very long string that we want to reverse efficiently! " * 1000
    print(f"Original string length: {len(long_string)} characters")
    
    # Reverse using the most efficient method
    reversed_long = reverse_string_slicing(long_string)
    print(f"Reversed string length: {len(reversed_long)} characters")
    print(f"First 50 characters of reversed string: {reversed_long[:50]}")
    print(f"Last 50 characters of reversed string: {reversed_long[-50:]}")
    
    # Run the main demonstration
    print("\n" + "=" * 60)
    main()