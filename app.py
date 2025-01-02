from flask import Flask, render_template
import sqlite3
import datetime

app = Flask(__name__)

@app.route('/report/<datetime>')
def report(datetime):
    date, time = datetime.split(' ')

    conn = sqlite3.connect('data.db')
    c = conn.cursor()
    c.execute("INSERT INTO detections (datetime) VALUES (?)", (datetime,))
    conn.commit()
    conn.close()

    return ""

@app.route('/')
def index():
    return render_template('index.html')

@app.route('/detections/<string:period>')
def detections(period):
    conn = sqlite3.connect('data.db')
    c = conn.cursor()
    
    # Fetch the latest datetime from the database
    c.execute("SELECT MAX(datetime) FROM detections")
    last_datetime = c.fetchone()[0]
    
    if last_datetime is None:
        return "No records found"
    
    last_datetime = datetime.datetime.strptime(last_datetime, '%Y-%m-%d %H:%M:%S')
    
    if period == '10min':
        delta = datetime.timedelta(minutes=10)
    elif period == '30min':
        delta = datetime.timedelta(minutes=30)
    elif period == '1hour':
        delta = datetime.timedelta(hours=1)
    else:
        return "Invalid period"

    start_time = (last_datetime - delta).strftime('%Y-%m-%d %H:%M:%S')

    query = "SELECT * FROM detections WHERE datetime(datetime) >= datetime(?)"
    c.execute(query, (start_time,))
    detections = c.fetchall()
    conn.close()

    return render_template('detections.html', detections=detections, period=period)

if __name__ == '__main__':
    app.run(debug=True)
