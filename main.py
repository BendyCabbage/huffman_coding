import math

def main():
    probabilities = input("Enter the symbols probabilities separated by a comma (,): ").split(",")
    probabilities = [float(i) for i in probabilities]
    probabilities.sort(reverse = True)
    radix = int(input("Enter the radix: "))
    encode(probabilities, radix)

# Given a list of probabilities in descending order and radix
# Uses Huffman's Algorithm to output the symbols with smallest average word length
def encode(probabilities, radix):
    average_word_length = 0
    huffman_symbols = probabilities[:]

    total_symbols = len(huffman_symbols)
    num_dummies = (1 - total_symbols) % (radix - 1)
    for i in range(num_dummies):
        huffman_symbols.append(0)
    total_symbols += num_dummies

    total_steps = math.ceil(total_symbols / (radix - 1)) - 1
    for i in range(total_steps):
        symbols_to_sum = min(radix, total_symbols)
        current_sum = 0
        for j in range(symbols_to_sum):
            current_sum += huffman_symbols.pop()

        total_symbols -= symbols_to_sum - 1
        average_word_length += current_sum
        place_symbol_high(huffman_symbols, current_sum)     

    print(f"Average word length: {average_word_length}")

#Insert into the list at the first possible time that maintains the list being sorted
def place_symbol_high(huffman_symbols, symbol_to_insert):
    if (len(huffman_symbols) == 0):
        huffman_symbols.append(symbol_to_insert)

    if (symbol_to_insert >= huffman_symbols[0]):
        huffman_symbols.insert(0, symbol_to_insert)
        return

    for i in range(1, len(huffman_symbols)):
        if (symbol_to_insert < huffman_symbols[i - 1] and symbol_to_insert >= huffman_symbols[i]):
            huffman_symbols.insert(i, symbol_to_insert)
            return

    huffman_symbols.append(symbol_to_insert)
    return

main()