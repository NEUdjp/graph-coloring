#include <iostream>
#include <fstream>
#include <time.h>
#include <stdlib.h>
#include <windows.h>
using namespace std;


//全局变量

clock_t starttime,endtime;
int **adjacent_color_table;
int **tabu_table;
int **adjacent_matrix;
int *vertex_color;

int fs = 0;
int f_best = 0;
int epoch = 0;
int N_node;
const int K = 50;
string file_input = "E:\\tmp\\data\\DSJC500.5.col";
const int N = 1; //统计次数
int Ni = 0; // 0 - N-1
int epoch_array[N];
double time_array[N];

void init(string filename)
{
    string tmp_s;
    bool mark = 0;  //0 则还没到边的行 1则到了
    int first,second;
    int tmp_n;
    ifstream infile;

    infile.open(filename, ios::in);
    if(!infile.is_open())
        cout<<"open file error"<<endl;
    else{}

    while(!infile.eof())
    {
        if(mark == 0)
        {
            infile >>tmp_s ;
            if(tmp_s == "edge" )
            {
                mark = 1;
                infile >> N_node;
                infile >> tmp_n;

                /* 得到顶点数量 N_node 初始化各种二维数组*/
                adjacent_color_table = new int*[N_node];
                tabu_table = new int*[N_node];
                adjacent_matrix = new int*[N_node];
                vertex_color = new int[N_node]();
                srand((int)time(NULL));

                for(int i = 0; i < N_node; i ++)
                {
                    adjacent_color_table[i] = new int[K]();
                    tabu_table[i] = new int[K]();
                    adjacent_matrix[i] = new int[N_node]();
                    vertex_color[i] = rand() % K;
                }
                /* 初始化结束 */

            }
        }
        else
        {
            infile >> tmp_s ;
            infile >> first >> second;
            /* 初始化邻接矩阵 */
            first --; second --;
            adjacent_matrix[first][second] = 1;
            /* 初始化结束 */
//            cout<<first<<second<<endl;
        }
    }
    infile.close();

    /* 邻接颜色表初始化  fs初始化*/
    for(int i = 0; i < N_node; i++)
        for(int j = 0; j < N_node; j++)
        {
            if(adjacent_matrix[i][j] == 1)
                adjacent_color_table[i][vertex_color[j]] ++;
            else if(adjacent_matrix[j][i] == 1)
                adjacent_color_table[i][vertex_color[j]] ++;
            else{}
        }

    for(int i =0 ; i < N_node; i ++)
        for(int j = 0; j <N_node; j++)
        {
            if(adjacent_matrix[i][j] == 1)
            {
                if (vertex_color[i] == vertex_color[j])
                    fs++;
                else {}
            }
            else{}
        }
    f_best = fs;
//    cout<<"fs:"<<fs<<endl;
    /* 邻接颜色表初始化完成  fs初始化完成*/

}

void move()
{
    starttime = clock();
    /* 开始迭代 */
    while (1)
    {
        epoch ++;
        //需要记录的数据: u i j delta u[0,N] i[0,k] j[0,k]
        //直接设置三个数组分别存u i j

        int tabu_array[2000][2];
        int non_tabu_array[2000][2];
        int tmp_delta = 0;
        int num_tabu = 0;
        int num_non_tabu = 0;
        int min_tabu_delta = 999999;
        int min_nontabu_delta = 999999;

        for(int vertex_index = 0; vertex_index < N_node; vertex_index++)
            if(adjacent_color_table[vertex_index][vertex_color[vertex_index]] > 0)
            {
                for(int color = 0; color < K; color ++)
                {
                    if(color != vertex_color[vertex_index])
                    {
                        tmp_delta = adjacent_color_table[vertex_index][color] - adjacent_color_table[vertex_index][vertex_color[vertex_index]] ;
                        if(tabu_table[vertex_index][color] >= epoch)
                        {
                            if(tmp_delta < min_tabu_delta)
                            {
                                num_tabu = 1;
                                tabu_array[0][0] = vertex_index;
                                tabu_array[0][1] = color;
                                min_tabu_delta = tmp_delta;
                            }
                            else if(tmp_delta == min_tabu_delta)
                            {
                                num_tabu ++;
                                tabu_array[num_tabu-1][0] = vertex_index;
                                tabu_array[num_tabu-1][1] = color;
                            }
                            else{}
                        }
                        else
                        {
                            if(tmp_delta < min_nontabu_delta)
                            {
                                num_non_tabu = 1;
                                non_tabu_array[0][0] = vertex_index;
                                non_tabu_array[0][1] = color;
                                min_nontabu_delta = tmp_delta;
                            }
                            else if(tmp_delta == min_nontabu_delta)
                            {
                                num_non_tabu ++;
                                non_tabu_array[num_non_tabu-1][0] = vertex_index;
                                non_tabu_array[num_non_tabu-1][1] = color;
                            }
                            else{}
                        }
                    }
                    else{}
                }
            }

        int best_delta = 999999;
        int best_u = 0;
        int best_i = 0;
        int best_j = 0;
        srand((int)time(NULL));
        if(min_tabu_delta < min_nontabu_delta && min_tabu_delta + fs < f_best)
        {
            best_delta = min_tabu_delta;
            int tmp_index = rand() % num_tabu;
            best_u = tabu_array[tmp_index][0];
            best_i = vertex_color[best_u];
            best_j = tabu_array[tmp_index][1];
        }
        else
        {
            best_delta = min_nontabu_delta;
            int tmp_index = rand() % num_non_tabu;
            best_u = non_tabu_array[tmp_index][0];
            best_i = vertex_color[best_u];
            best_j = non_tabu_array[tmp_index][1];
        }


        /* 更新操作 */
        vertex_color[best_u] = best_j;
        fs = fs + best_delta;
        if(fs < f_best)
            f_best = fs;
        else{}
        tabu_table[best_u][best_i] = epoch + fs + rand() % 10;
        for(int i = 0; i < N_node; i ++)
        {
            if(adjacent_matrix[i][best_u] == 1 || adjacent_matrix[best_u][i] == 1)
            {
                adjacent_color_table[i][best_i] -= 1;
                adjacent_color_table[i][best_j] += 1;
            }
            else{}
        }
        if(fs == 0)
        {
            break;
        }
//        cout<<fs<<' ';
    }
    /* 结束迭代 */
    endtime = clock();

    cout<<"sum epoch:"<<epoch<<endl;
    cout<<"sum time:"<<(endtime - starttime)<<endl;

    epoch_array[Ni] = epoch;
    epoch = 0;
    time_array[Ni] = (double)(endtime-starttime);
    Ni++;

}

void free_memory()
{
    delete []vertex_color;
    for(int i = 0; i < N_node; i++)
    {
        delete []adjacent_color_table[i];
        delete []tabu_table[i];
        delete []adjacent_matrix[i];
    }
    delete []adjacent_color_table;
    delete []tabu_table;
    delete []adjacent_matrix;

}


int main() {
    for(int k = 0; k < N; k++)
    {
        cout<<"No."<<k+1<<endl;
        init(file_input);
        move();
        free_memory();
        Sleep(5000);
    }

    // 计算最大最小平均
    double max_time = time_array[0];
    double min_time = time_array[0];
    double avg_time = 0;
    int avg_epoch = 0;
    for(int k = 0; k < N; k++)
    {
        if(time_array[k] > max_time)
            max_time = time_array[k];
        if(time_array[k] < min_time)
            min_time = time_array[k];
        avg_time += time_array[k];
    }
    avg_time = avg_time / N;

    for(int k = 0; k < N; k++)
    {
        avg_epoch += epoch_array[k];
    }
    avg_epoch = avg_epoch / N;

    cout<<"max_time:"<<max_time<<" min_time:"<<min_time<<" avg_time:"<<avg_time<<endl;
    cout<<"avg_epoch:"<<avg_epoch<<endl;
    return 0;
}
