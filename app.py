import os
import subprocess
from flask import Flask, render_template, request, redirect, url_for, flash

app = Flask(__name__)
app.secret_key = 'super-secret-key'

ANALYZER_EXE = "./analyzer"

def run_analyzer(args):
    try:
        result = subprocess.run([ANALYZER_EXE] + args, capture_output=True, text=True)
        return result.stdout.strip()
    except Exception as e:
        print(f"Error running analyzer: {e}")
        return ""

@app.route("/")
def index():
    # Get List
    output = run_analyzer(["list"])
    students = []
    if output:
        for line in output.split("\n"):
            parts = line.split("|")
            if len(parts) == 8:
                students.append({
                    'regNo': parts[0],
                    'name': parts[1],
                    'marks': [parts[2], parts[3], parts[4]],
                    'total': parts[5],
                    'average': parts[6],
                    'grade': parts[7]
                })
    
    # Get Stats
    stats_output = run_analyzer(["stats"])
    stats = {}
    if stats_output:
        parts = stats_output.split("|")
        if len(parts) == 8:
            stats = {
                'class_average': parts[0],
                'highest_name': parts[1],
                'highest_total': parts[2],
                'lowest_total': parts[3],
                'grade_dist': {'A': parts[4], 'B': parts[5], 'C': parts[6], 'Fail': parts[7]}
            }
            
    return render_template("index.html", students=students, stats=stats)

@app.route("/add", methods=["POST"])
def add_student():
    reg_no = request.form.get("regNo")
    name = request.form.get("name")
    m1 = request.form.get("m1")
    m2 = request.form.get("m2")
    m3 = request.form.get("m3")
    
    output = run_analyzer(["add", reg_no, name, m1, m2, m3])
    if "SUCCESS" in output:
        flash("Record added successfully!")
    else:
        flash("Error adding record.")
    return redirect(url_for("index"))

@app.route("/delete/<int:reg_no>")
def delete_student(reg_no):
    output = run_analyzer(["delete", str(reg_no)])
    if "SUCCESS" in output:
        flash("Record deleted.")
    return redirect(url_for("index"))

@app.route("/update", methods=["POST"])
def update_marks():
    reg_no = request.form.get("regNo")
    m1 = request.form.get("m1")
    m2 = request.form.get("m2")
    m3 = request.form.get("m3")
    
    output = run_analyzer(["update", reg_no, m1, m2, m3])
    if "SUCCESS" in output:
        flash("Marks updated successfully!")
    else:
        flash("Error: Student not found.")
    return redirect(url_for("index"))

@app.route("/search")
def search():
    reg_no = request.args.get("regNo")
    if not reg_no:
        return redirect(url_for("index"))
    
    output = run_analyzer(["list"])
    students = []
    if output:
        for line in output.split("\n"):
            parts = line.split("|")
            if len(parts) == 8 and parts[0] == reg_no:
                students.append({
                    'regNo': parts[0],
                    'name': parts[1],
                    'marks': [parts[2], parts[3], parts[4]],
                    'total': parts[5],
                    'average': parts[6],
                    'grade': parts[7]
                })
                break
    
    return render_template("index.html", students=students, stats={}, search_query=reg_no)

if __name__ == "__main__":
    app.run(host="0.0.0.0", port=int(os.environ.get("PORT", 5000)))
