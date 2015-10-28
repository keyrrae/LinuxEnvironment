import os


print "The source code is:"
os.system("cat source.asm")

print
print

f = open("./source.asm", 'r')
d = open("./target", 'w')

command_dict = {"WRITE": '0', "READ": '1', "ADD": '2', "SUB": '3', "END": '4'}

lines = f.readlines()
f.close()

lines = [l.strip() for l in lines]

state = "command"

memlst = ["00" for i in xrange(16)]

memnum = 0

for l in lines:
    if l == "----":
        memlst[memnum] = "40"
        state = "memory"
    else:
        split_tmp = l.split(' ')
        split_line = []
        for ele in split_tmp:
            if ele != '':
                split_line.append(ele)
        #print split_line
        if state == "command":
            val_com = split_line[1].strip("0x0")
            #print "val_come ", val_com
            memlst[memnum] = command_dict[split_line[0]] + val_com
            #print memlst[memnum]
            memnum = memnum + 1
        else:
            val_mem = int(split_line[1], 0)
            if val_mem < 16:
                memlst[int(split_line[0], 0)] = '0' + format(val_mem, 'x')
            else:
                memlst[int(split_line[0], 0)] = format(val_mem, 'x')


out_string = ''.join(memlst)

d.write(out_string)
d.close()

print "The assembled code is:"
os.system("cat target")

print
