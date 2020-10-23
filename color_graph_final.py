import random
import time
def color_graph(path_input,k_input):
    """主函数"""

    """读文件"""
    data_path = path_input
    content = ""
    with open(data_path, 'r', encoding='utf-8') as f:
        content = f.readlines()

    edge_line = 0
    n_vertex = 0
    n_edge = 0

    for i in range(len(content)):
        if(content[i].startswith('p')):
            n_vertex = int(content[i].split()[2])
            n_edge   = int(content[i].split()[3]) // 2
            edge_line= i + 1
            break
        else:
            continue
    # 注意一下文件中下标是1开始的，数组中下标是0开始的 暂定文件中所有下标减一

    """初始化邻接矩阵"""
    K = k_input # 颜色 0到K-1
    fs = 0 # 目标函数
    epoch = 0
    f_best = 0 # 历史最优解
    adjacent_color_table = [[0 for i in range(K)] for j in range(n_vertex)]
    tabu_table           = [[0 for i in range(K)] for j in range(n_vertex)]
    adjacent_matrix = [[0 for i in range(n_vertex)] for j in range(n_vertex)]

    for i in range(edge_line,len(content)):
        first  = int(content[i].split()[1])
        second = int(content[i].split()[2])
        first  -= 1
        second -= 1
        adjacent_matrix[first][second] = 1
        # adjacent_matrix[second][first] = 1
        # ex: e 6 2 -> adjacent_matrix[5][1] = 1

    """初始化颜色表，计算冲突表"""
    vertex_color = [0] * n_vertex
    for i in range(n_vertex):
        vertex_color[i] = random.randint(0,K-1)
    for i in range(n_vertex):
        for j in range(n_vertex):
            if(adjacent_matrix[i][j] == 1):
                adjacent_color_table[i][vertex_color[j]] += 1
            elif(adjacent_matrix[j][i] == 1):
                adjacent_color_table[i][vertex_color[j]] += 1
            else:
                pass

    """初始化fs"""
    for i in range(n_vertex):
        for j in range(n_vertex):
            if(adjacent_matrix[i][j] == 1):
                if(vertex_color[i] == vertex_color[j]):
                    fs += 1
                else:
                    pass
            else:
                continue

    f_best = fs


    """
    -------------------完成全部初始化------------------------
    """


    # 这个类的delta改成init计算 实在不行就取消（应该没人会在意python优化了多少吧）
    class Move():
        """move<u,i,j>  u节点从i颜色移到j颜色"""
        def __init__(self, u, i, j):
            self.u = u
            self.i = i
            self.j = j
            self.delta = 0

        def get_delta(self, adjacent_color_table_i):
            self.delta = adjacent_color_table_i[self.u][self.j] - adjacent_color_table_i[self.u][self.i]

    begin_time = time.time()

    """开始迭代"""
    while(True):
        epoch += 1
        tabu_list = []
        non_tabu_list = []

        # 计算所有move，找到最优变化量
        # 若有多个最优，则都存到列表中，再根据概率选择
        # 禁忌最优必须优于当前且优于历史(需要查tabu表) 最后再查就行吧

        for vertex_index in range(n_vertex):
            if(adjacent_color_table[vertex_index][vertex_color[vertex_index]] > 0) :#这个是不冲突的意思
                for color in range(K):
                    if(color != vertex_color[vertex_index]):
                        tmp_m = Move(vertex_index, vertex_color[vertex_index], color)
                        tmp_m.get_delta(adjacent_color_table)

                        if(tabu_table[tmp_m.u][tmp_m.j] >= epoch): # 这块是大于号
                            # 更新tabu_list 即已经解禁
                            if(len(tabu_list) == 0):
                                tabu_list.append(tmp_m)
                            else:
                                if(tmp_m.delta < tabu_list[0].delta):
                                    tabu_list.clear()
                                    tabu_list.append(tmp_m)
                                elif(tmp_m.delta == tabu_list[0].delta):
                                    tabu_list.append(tmp_m)
                                else:
                                    pass
                        else:
                            # 更新non-tabu_list 即还未解禁
                            if(len(non_tabu_list) == 0):
                                non_tabu_list.append(tmp_m)
                            else:
                                if(tmp_m.delta < non_tabu_list[0].delta):
                                    non_tabu_list.clear()
                                    non_tabu_list.append(tmp_m)
                                elif(tmp_m.delta == non_tabu_list[0].delta):
                                    non_tabu_list.append(tmp_m)
                                else:
                                    pass
        # print("长度", len(tabu_list), ' ', len(non_tabu_list))

        """在tabu list 和 non tabu list 找最优"""
        if(len(tabu_list) > 0 and len(non_tabu_list) > 0):
            # print(tabu_list[0].delta, non_tabu_list[0].delta)
            if(tabu_list[0].delta + fs < f_best):
                if(tabu_list[0].delta < non_tabu_list[0].delta):
                    best_move = tabu_list[random.randint(0, len(tabu_list) - 1)]
                elif(tabu_list[0].delta > non_tabu_list[0].delta):
                    best_move = non_tabu_list[random.randint(0, len(non_tabu_list) - 1)]
                else:
                    tmp_index = random.randint(0, len(tabu_list) + len(non_tabu_list) -1)
                    if(tmp_index // len(tabu_list) == 0):
                        best_move = tabu_list[tmp_index % len(tabu_list)]
                    else:
                        best_move = non_tabu_list[tmp_index % len(tabu_list)]
            else:
                best_move = non_tabu_list[random.randint(0,len(non_tabu_list)-1)]
        elif(len(tabu_list) > 0 and len(non_tabu_list) == 0):
            best_move = tabu_list[random.randint(0, len(tabu_list) - 1)]
        elif(len(tabu_list) ==0 and len(non_tabu_list) > 0):
            best_move = non_tabu_list[random.randint(0, len(non_tabu_list) - 1)]
        else:
            print("error")
            break


        # 更新操作
        # 更新tabu表和adjacent_color表和vertex_color表
        # 更新fs f_best
        vertex_color[best_move.u] = best_move.j
        fs = fs + best_move.delta
        if(fs < f_best):
            f_best = fs
        else:
            pass
        tabu_table[best_move.u][best_move.i] = epoch + fs + random.randint(1,10)
        for vertex in range(n_vertex):
            if (adjacent_matrix[vertex][best_move.u] == 1 or adjacent_matrix[best_move.u][vertex] == 1):
                adjacent_color_table[vertex][best_move.i] -= 1
                adjacent_color_table[vertex][best_move.j] += 1
            else:
                pass


        if(fs == 0 or fs < 0):
            # print("success")
            break


        print("epoch:",epoch,end=' ')
        print("fs_best:", f_best,end=' ')
        print("当前fs:",fs)

    end_time = time.time()
    sum_time = end_time - begin_time
    # print(sum_time)
    return sum_time,epoch

if __name__ == '__main__':
    cishu = 1
    time_list = []
    epoch_list = []
    sumoftime = 0
    sumofepoch = 0
    for i in range(cishu):
        use_time,use_epoch = color_graph("../data/DSJC500.1.col",12)
        time_list.append(use_time)
        epoch_list.append(use_epoch)
        sumoftime += use_time
        sumofepoch += use_epoch
        print(i+1)

    time_list.sort()
    epoch_list.sort()

    print("最小epoch",epoch_list[0],"最大epoch",epoch_list[cishu-1],"平均epoch",sumofepoch/cishu)
    print("最短时间",time_list[0],"最长时间",time_list[cishu-1],"平均时间",sumoftime/cishu)

