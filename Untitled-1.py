

arr = [1, 2, 3, 0]

recheck = True

while recheck == True:
    recheck = False
    for i in range(0, len(arr)):
        if arr[i] > arr[i+1]:
            temp1 = arr[i]
            temp2 = arr[i+1]
            arr[i] = temp2
            arr[i+1] = temp1
        else:
            continue
        if (i!=0):
            if arr[i] < arr[i-1]:
                recheck = True

