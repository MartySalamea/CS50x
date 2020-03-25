from cs50 import get_string
import re


def get_Coleman_liau_index(letters, words, sentences):
    L = 100 * letters / words
    S = 100 * sentences / words
    return round(0.0588 * L - 0.296 * S - 15.8)


def main():
    text = get_string('Text: ')
    letters = len(list(filter(lambda c: c.lower().isalpha(), text)))
    words = len(text.split())
    sentences = len(re.findall(r'[.?!]', text))
    grade = get_Coleman_liau_index(letters, words, sentences)

    if grade < 1:
        print('Before Grade 1')
    elif grade > 16:
        print('Grade 16+')
    else:
        print(f'Grade {grade}')


if __name__ == '__main__':
    main()
