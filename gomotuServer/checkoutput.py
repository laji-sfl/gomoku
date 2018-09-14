#检查输出的fd个数是否重复或漏掉，以此来简单判断是否出错
myfile = open("./output.txt")
everyline = myfile.readlines();
maxfd = input("max fd num:")
fdnum = [0] * int(maxfd);#每次自己设置文件描述符的最大值
for line in everyline:
    tmp = line.split(',')
    fdnum[int(tmp[0][3:])] += 1
i = 0;
start = input("start fd num:")
print("answer:")
for num in fdnum:
    if num != 1 and i >= int(start):
        print(i)
    i += 1
