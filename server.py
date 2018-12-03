from flask import Flask, render_template
from parser import parse_map
from parser import get_addrs
import sys

app = Flask(__name__)

# rlist = []
# # returns a list of effected variables from a race condition
# rlist = parse_map()

# for r in rlist:
	# print r
@app.route("/results")
def results():
	rlist = []
	addrlist = []

	rlist = parse_map(str(sys.argv[1]))
	addrlist = get_addrs(str(sys.argv[1]))
	if not rlist:
		return render_template('empty_list.html')
	return render_template('show_list.html', data=rlist)

@app.route("/")
def index():
	return render_template('home.html') 

if __name__ == '__main__':
	app.run(debug=True)