import os

d = input("Directory path ")

files = os.listdir(d)
os.chdir(d)
newNameRoot =  ['11','12','13','14',
				'21','22','23','24',
				'31','32','33','34',
				'41','42','43','44',]
newNameExtension = '.wav';
index = 0;
for f in files:
	print("Change ", f , "in " ,newNameRoot[index]+newNameExtension)
	index += 1
	
if (input("Rename file? [y/n]") == 'y'):
	print("Renaming...")
	index = 0
	for f in files:
		os.rename(f, newNameRoot[index]+newNameExtension)
		index += 1
	
else:
	print("Canceled")
print("Process finished")
