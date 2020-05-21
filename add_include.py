with open('parser.tab.h', 'r+') as f:
	content = f.read()
	f.seek(0, 0)
	f.write('#include "tree.h"\n' + content)
