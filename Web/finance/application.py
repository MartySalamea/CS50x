import os
import datetime

from cs50 import SQL
from flask import Flask, flash, jsonify, redirect, render_template, request, session
from flask_session import Session
from tempfile import mkdtemp
from werkzeug.exceptions import default_exceptions, HTTPException, InternalServerError
from werkzeug.security import check_password_hash, generate_password_hash

from helpers import apology, login_required, lookup, usd

# Configure application
app = Flask(__name__)

# Ensure templates are auto-reloaded
app.config["TEMPLATES_AUTO_RELOAD"] = True

# Ensure responses aren't cached
@app.after_request
def after_request(response):
    response.headers["Cache-Control"] = "no-cache, no-store, must-revalidate"
    response.headers["Expires"] = 0
    response.headers["Pragma"] = "no-cache"
    return response

# Custom filter
app.jinja_env.filters["usd"] = usd

# Configure session to use filesystem (instead of signed cookies)
app.config["SESSION_FILE_DIR"] = mkdtemp()
app.config["SESSION_PERMANENT"] = False
app.config["SESSION_TYPE"] = "filesystem"
Session(app)

# Configure CS50 Library to use SQLite database
db = SQL("sqlite:///finance.db")

# Make sure API key is set
if not os.environ.get("API_KEY"):
    raise RuntimeError("API_KEY not set")


@app.route("/")
@login_required
def index():

    # Get user's ID
    uid = session["user_id"]

    # Query database for username
    transactions = db.execute("SELECT * FROM transactions WHERE user_id = :uid",
                      uid=uid)

    # Query database for user
    rows = db.execute("SELECT * FROM users WHERE id = :uid",
                      uid=uid)

    # Start a grand total with the user's cash balance
    cash = rows[0]["cash"]
    sum = 0

    stocks = {}

    for transaction in transactions:
        symbol = transaction["symbol"]
        shares = transaction["shares"]
        type = transaction["type"]
        quote = lookup(symbol)

        # Check quote validity
        if not quote:
            return apology("invalid symbol in transactions", 403)

        price = quote["price"]
        name = quote["name"]

        if not symbol in stocks:
            stocks[symbol] = { "shares": 0 }

        stocks[symbol]["current_price"] = price
        stocks[symbol]["name"] = name

        if type == "buy":
            stocks[symbol]["shares"] += shares
            sum += price * shares

        else:
            stocks[symbol]["shares"] -= shares
            sum -= price * shares

    grand_total = cash + sum

    return render_template("index.html", stocks=stocks, grand_total=grand_total, cash=cash)


@app.route("/buy", methods=["GET", "POST"])
@login_required
def buy():

    # User reached route via POST (as by submitting a form via POST)
    if request.method == "POST":

        symbol = request.form.get("symbol")
        shares = request.form.get("shares")
        uid = session["user_id"]
        now = datetime.datetime.now()

        # Ensure symbol was submitted
        if not symbol:
            return apology("must provide symbol", 403)

        # Ensure shares was submitted
        if not shares:
            return apology("must provide shares", 403)

        # Cast shares to int
        shares = int(shares)

        # Check share number validity
        if shares < 1:
            return apology("must provide a valid number of shares", 403)

        # Lookup quote
        quote = lookup(symbol)

        # Check symbol validity
        if not quote:
            return apology("must provide a valid symbol", 403)

        price = quote["price"]
        name = quote["name"]
        cost = price * shares

        # Query database for user
        rows = db.execute("SELECT * FROM users WHERE id = :uid",
                          uid=uid)

        # Get the user's cash balance
        cash = rows[0]["cash"]

        # Check the user's balance
        if cash < cost:
            return apology("not enough funds", 403)

        # Create a new transaction
        db.execute("INSERT INTO transactions (user_id, symbol, name, shares, price, type, purchased_at) VALUES (:uid, :symbol, :name, :shares, :price, :type, :date)",
                          uid=uid, symbol=symbol, name=name, shares=shares, price=price, type="buy", date=now)

        # Update the user's balance
        db.execute("UPDATE users SET cash = :balance WHERE id = :uid",
                          balance=cash-cost, uid=uid)

        # Redirect user to home page
        return redirect("/")

    # User reached route via GET (as by clicking a link or via redirect)
    else:
        return render_template("buy.html")


@app.route("/history")
@login_required
def history():

    # Get user's ID
    uid = session["user_id"]

    # Query database for username
    transactions = db.execute("SELECT * FROM transactions WHERE user_id = :uid ORDER BY purchased_at DESC",
                      uid=uid)

    return render_template("history.html", transactions=transactions)


@app.route("/login", methods=["GET", "POST"])
def login():
    """Log user in"""

    # Forget any user_id
    session.clear()

    # User reached route via POST (as by submitting a form via POST)
    if request.method == "POST":

        username = request.form.get("username")
        password = request.form.get("password")

        # Ensure username was submitted
        if not username:
            return apology("must provide username", 403)

        # Ensure password was submitted
        elif not password:
            return apology("must provide password", 403)

        # Query database for username
        rows = db.execute("SELECT * FROM users WHERE username = :username",
                          username=username)

        # Ensure username exists and password is correct
        if len(rows) != 1 or not check_password_hash(rows[0]["hash"], password):
            return apology("invalid username and/or password", 403)

        # Remember which user has logged in
        session["user_id"] = rows[0]["id"]

        # Redirect user to home page
        return redirect("/")

    # User reached route via GET (as by clicking a link or via redirect)
    else:
        return render_template("login.html")


@app.route("/logout")
def logout():
    """Log user out"""

    # Forget any user_id
    session.clear()

    # Redirect user to login form
    return redirect("/")


@app.route("/quote", methods=["GET", "POST"])
@login_required
def quote():
    """Get stock quote."""

    # User reached route via POST (as by submitting a form via POST)
    if request.method == "POST":

        symbol = request.form.get("symbol")

        # Ensure symbol was submitted
        if not symbol:
            return apology("must provide symbol", 403)

        # Lookup quote
        quote = lookup(symbol)

        # Check symbol validity
        if not quote:
            return apology("must provide a valid symbol", 403)

        # Redirect user to home page
        return render_template("quoted.html", quote=quote)

    # User reached route via GET (as by clicking a link or via redirect)
    else:
        return render_template("quote.html")


@app.route("/register", methods=["GET", "POST"])
def register():
    """Register user"""

    # User reached route via POST (as by submitting a form via POST)
    if request.method == "POST":

        username = request.form.get("username")
        password = request.form.get("password")
        confirmation = request.form.get("confirmation")

        # Ensure username was submitted
        if not username:
            return apology("must provide username", 403)

        # Ensure password was submitted
        elif not password:
            return apology("must provide password", 403)

        # Ensure confirmation password was submitted
        elif not confirmation:
            return apology("must provide confirmation password", 403)

        # Ensure password and confirmation are the same
        if (password != confirmation):
            return apology("password must match confirmation password", 403)

        # Find existing users with given username
        existing_user = db.execute("SELECT * FROM users WHERE username = :username",
                  username=username)

        # Check username is not taken
        if len(existing_user) > 0:
            return apology("please provide a unique username", 403)

        # Query database for username
        uid = db.execute("INSERT INTO users (username, hash) VALUES (:name, :password)",
                          name=username, password=generate_password_hash(password))

        # Remember which user has logged in
        session["user_id"] = uid

        # Redirect user to home page
        return redirect("/")

    # User reached route via GET (as by clicking a link or via redirect)
    else:
        return render_template("register.html")


@app.route("/sell", methods=["GET", "POST"])
@login_required
def sell():

     # Get user's ID
    uid = session["user_id"]

    # Query database for username
    transactions = db.execute("SELECT * FROM transactions WHERE user_id = :uid",
                  uid=uid)

    # User reached route via POST (as by submitting a form via POST)
    if request.method == "POST":

        symbol = request.form.get("symbol")
        shares = request.form.get("shares")
        now = datetime.datetime.now()

        # Ensure symbol was submitted
        if not symbol:
            return apology("must provide symbol", 403)

        # Ensure shares was submitted
        if not shares:
            return apology("must provide shares", 403)

        # Cast shares to int
        shares = int(shares)

        # Check share number validity
        if shares < 1:
            return apology("must provide a valid number of shares", 403)

        # Lookup quote
        quote = lookup(symbol)

        # Check quote validity
        if not quote:
            return apology("must provide a valid symbol", 403)

        price = quote["price"]
        name = quote["name"]
        cost = price * shares

        # Query database for user
        rows = db.execute("SELECT * FROM users WHERE id = :uid",
                          uid=uid)

        # Get the user's cash balance
        cash = rows[0]["cash"]

        # Get total buys and sells
        buys = db.execute("SELECT SUM(shares) FROM transactions WHERE user_id = :uid AND type = :type",
                            uid=uid, type="buy")
        sells = db.execute("SELECT SUM(shares) FROM transactions WHERE user_id = :uid AND type=:type",
                            uid=uid, type="sell")
        buys = buys[0]["SUM(shares)"]
        sells = sells[0]["SUM(shares)"]

        # Set buys and sells to 0 if they don't exist
        if not buys:
            buys = 0
        if not sells:
            sells = 0

        # Check if user does not own that many shares of the stock.
        if (buys - sells) < shares:
            return apology("you don't have enough of these shares to sell", 403)

        # Create a new transaction
        db.execute("INSERT INTO transactions (user_id, symbol, name, shares, price, type, purchased_at) VALUES (:uid, :symbol, :name, :shares, :price, :type, :date)",
                          uid=uid, symbol=symbol, name=name, shares=shares, price=price, type="sell", date=now)

        # Update the user's balance
        db.execute("UPDATE users SET cash = :balance WHERE id = :uid",
                          balance=cash+cost, uid=uid)

        # Redirect user to home page
        return redirect("/")

    # User reached route via GET (as by clicking a link or via redirect)
    else:

        symbols = set()

        for transaction in transactions:
            symbol = transaction["symbol"]
            symbols.add(symbol)

        return render_template("sell.html", symbols=symbols)

@app.route("/add", methods=["GET", "POST"])
@login_required
def add():

    # User reached route via POST (as by submitting a form via POST)
    if request.method == "POST":

        funds = request.form.get("funds")
        uid = session["user_id"]

        # Ensure funds was submitted
        if not funds:
            return apology("must provide funds", 403)

        # Cast funds to int
        funds = int(funds)

        # Check funds number validity
        if funds < 1:
            return apology("must provide a valid amount for funds", 403)

        # Query database for user
        rows = db.execute("SELECT * FROM users WHERE id = :uid",
                          uid=uid)

        # Get the user's cash balance
        cash = rows[0]["cash"]

        # Update the user's balance
        db.execute("UPDATE users SET cash = :balance WHERE id = :uid",
                          balance=cash+funds, uid=uid)

        # Redirect user to home page
        return redirect("/")

    # User reached route via GET (as by clicking a link or via redirect)
    else:
        return render_template("add.html")


def errorhandler(e):
    """Handle error"""
    if not isinstance(e, HTTPException):
        e = InternalServerError()
    return apology(e.name, e.code)


# Listen for errors
for code in default_exceptions:
    app.errorhandler(code)(errorhandler)
