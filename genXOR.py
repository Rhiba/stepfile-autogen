import random

def gen():
	f = open('testdata','w')
	for num in range(1,20000):
		ran = random.random()
		if ran < 0.25:
			f.write("0 0 0\n")
		elif ran < 0.5:
			f.write("0 1 1\n")
		elif ran < 0.75:
			f.write("1 0 1\n")
		else:
			f.write("1 1 0\n")

if __name__ == "__main__":
	gen()
