from cs50 import get_float

while True:         # input the sum required to change
    cash = get_float("Change owed: ")
    if cash > 0:
        break

# converting the sum (float value) into integer
cash_in_cents = int(cash * 100)

# coins to change set
coins = [25, 10, 5, 1]

coins_to_change = 0

for coin in coins:
    coins_to_change += cash_in_cents // coin
    cash_in_cents %= coin
    if cash_in_cents == 0:
        break

print(coins_to_change)