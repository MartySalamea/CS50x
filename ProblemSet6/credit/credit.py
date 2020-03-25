from cs50 import get_int


def main():
    num = get_int("Number: ")
    print(define_card(num))


def digits_sum(num):
    sum = 0
    while num > 0:
        sum += num % 10
        num //= 10
    return sum


def luhns_check(num):
    s = str(num)
    check_sum = 0
    for c in s[-2::-2]:
        check_sum += digits_sum(2 * int(c))
    for c in s[-1::-2]:
        check_sum += int(c)
    return check_sum % 10 == 0


def define_card(num):
    if not luhns_check(num):
        return "INVALID"

    num_str = str(num)
    start_binum = int(num_str[0:2])
    num_len = len(num_str)

    # All American Express numbers start with 34 or 37
    # American Express uses 15-digit numbers
    if num_len == 15 and (start_binum == 34 or start_binum == 37):
        return "AMEX"

    # most MasterCard numbers start with 51, 52, 53, 54, or 55...
    # MasterCard uses 16-digit numbers
    if num_len == 16 and 51 <= start_binum <= 55:
        return "MASTERCARD"

    # all Visa numbers start with 5
    # Visa uses 13- and 16-digit numbers
    if num_str[0] == '4' and (num_len == 13 or num_len == 16):
        return "VISA"

    return "INVALID"


if __name__ == "__main__":
    main()