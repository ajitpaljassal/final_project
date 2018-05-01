#include <iostream>
#include <fstream>
#include <vector>
#include <sstream>
#include <cstring>
#include <iomanip>
using namespace std;
typedef Maximum 500;
typedef unsigned long long ULL;
ULL currenttime=0;
class task_struct // define properties of nodes in graph
{
    vector<int> child;  // child nodes of this node
    vector<int> parent; // parent nodes of this node
    int soon = 1;       // control step in asap
    int late = 1;       // control step in alap
    int pt;             //in cycles
    ULL starttime=0;
    bitset<vertex> parent_b;
};

int vertex;              // total number of vertex

vector<task_struct> node;// array of every node information


enum EventType
{
    Task_END,COMM_END;
};

class top
{
    EventSimulator s;
    Hardware hw;            //calling its constructor sets the configuration of hardware
    GlobalSched g(s,hw);    //
    AppLauncher(g,s);
    //something maybe left

};

class Event
{
private:
    ULL time;
    RunableObj *obj;

    bool operator <(const Event &e)const
    {
        return time < e.time;
    }

    Event(RunableObj *o,ULL t,EventType type)
    {
        obj=o;
        time=t;
    }

};

virtual class RunableObj
{

    public virtual void run(EventType);
};

class EventSimulator
{
    priority_queue<Event> mq;

    setEvent(RunableObj *o,ULL t,EventType type)
    {
        Event *e=new Event(o,t,type);
        mq.push(*e);
    }

    Event nextEvent()
    {

        if(mq.size()==0)
        cout<<"NO EVENTS THERE\n";
        else
        mq.pop();
    }

    ULL mCurrTime;//WHY THIS IS NEEDED

    EventSimulator(){}

    void runSimulator()
    {
        while(1)
        {
            Event e=nextEvent();
            if(e.obj==NULL)
                break;
            e.obj->run(e.type);

        }

    }
};



class AppLauncher
{

public:
    int task_graph_input() // too many details in this function and this is not key point
    {
        string line, temp_string, filename;
        task_struct a;
        char temp_filename[20];
        cout << "Enter the name of the graph file: ";
        cin >> filename;
        strcpy(temp_filename, filename.c_str());
        ifstream myfile(temp_filename);

        if (myfile.is_open())
        {
            getline(myfile, line);
            stringstream ss(line);
            ss >> vertex;
            for (int i = 0; i < vertex; i++)
                node.push_back(a);
            while (getline(myfile, line))
            {
                ss.clear();
                ss.str(line);
                int stringlength = line.length();
                int temp_node, temp;
                ss >> temp_node;
                for (int idx = 1; idx < stringlength; idx++)
                {
                    ss >> temp;
                    if (idx > 1 && temp == node[temp_node].child[idx - 2])
                        break;
                    node[temp_node].child.push_back(temp);
                    node[temp].parent.push_back(temp_node);
                }
            }
        }
        else
        {

            cout << "Unable to open file\n";
            return -1;
        }
        return 0;
    }

};



class LocalSchedulerAndVirtual:public RunableObj
{
private :int *temp_step;
          int max_length = 1;

public: void asap(vector<task_struct> &node)
    {
        temp_step = new int[vertex]; // store current temporary step
        for (int i = 0; i < vertex; i++)
            temp_step[i] = 0;
        for (int idx = 0; idx < vertex; idx++)
        {
            if (node[idx].parent.empty())
            {
                node[idx].soon = 1;
                temp_step[idx] = 1;
                trav_down(node, idx);
            }
        }
        delete[] temp_step;
    }

    void alap(vector<task_struct> &node)
    {
        temp_step = new int[vertex]; // store current temporary step
        for (int i = 0; i < vertex; i++)
            temp_step[i] = 0;
        for (int idx = 0; idx < vertex; idx++)
        {
            if (node[idx].child.empty())
            {
                node[idx].late = 1;
                temp_step[idx] = 1;
                trav_up(node, idx);
            }
        }
        for (int i = 0; i < vertex; i++)
            node[i].late = max_length + 1 - node[i].late;
        delete[] temp_step;
        return;
    }

    void trav_down(vector<task_struct> &node, int idx)
    {
        for (int i = 0; i < node[idx].child.size(); i++) // go through all child nodes of current node
        {
            int child_num = node[idx].child[i];
            temp_step[child_num] = temp_step[idx] + 1;
            node[child_num].soon = max(temp_step[child_num], node[child_num].soon);
            trav_down(node, child_num);
        }
        return;
    }

    void trav_up(vector<task_struct> &node, int idx)
    {
        for (int i = 0; i < node[idx].parent.size(); i++) // go through all parent nodes of current node
        {
            int parent_num = node[idx].parent[i];
            temp_step[parent_num] = temp_step[idx] + 1;
            max_length = max(max_length, temp_step[parent_num]); // store the length of critical path
            node[parent_num].late = max(temp_step[parent_num], node[parent_num].late);
            trav_up(node, parent_num);
        }
        return;
    }

    void listScheduling(vector<task_struct> &node)
    {
        cout << "Enter the number of restriction of the CORES: ";
        int N;
        cin >> N;                                // initialize N by resource restrict
        vector<int> scheduled_list[N];           // N resource units
        vector<int> ScheduleNode(vertex, 0);     // nodes which is scheduled in previous control step
        vector<int> TempScheduleNode(vertex, 0); // nodes which is scheduled in current control step
        vector<int> thisCycleInvalid;            // mark the nodes whose precondition is not done
        int remainderNode = vertex;              // the number of remaind Nodes which is not scheduled
        struct                                   // tb denotes best node should be scheduled in current step
        {
            int soon;
            int late;
            int idx;
        } tb;
        while (remainderNode > 0)
        {
            for (int j = 0; j < N; j++)
            {
                int getvalid = 0;
                if (getvalid == -1)
                    continue;
                while (getvalid == 0)
                {
                    tb.soon = vertex + 1;
                    tb.late = vertex + 1;
                    for (int i = 0; i < vertex; i++)
                    {
                        bool flag = true;
                        for (auto e : thisCycleInvalid)
                        {
                            if (e == i)
                                flag = false;
                        }
                        if (!flag || TempScheduleNode[i] == 1)
                            continue;
                        if (node[i].late < tb.late)
                        {
                            tb.idx = i;
                            tb.soon = node[i].soon;
                            tb.late = node[i].late;
                        }
                        else if (node[i].late == tb.late && node[i].soon < tb.soon)
                        {
                            tb.idx = i;
                            tb.soon = node[i].soon;
                            tb.late = node[i].late;
                        }
                    }

                    if (tb.soon == vertex + 1)
                    {
                        getvalid = -1;
                        scheduled_list[j].push_back(-1);
                        break;
                    }
                    bool flag = true;
                    for (int i = 0; i < node[tb.idx].parent.size(); i++)
                    {
                        if (ScheduleNode[node[tb.idx].parent[i]] == 0)
                            flag = false;
                    }
                    if (flag)
                    {
                        TempScheduleNode[tb.idx] = 1;
                        remainderNode--;
                        scheduled_list[j].push_back(tb.idx);
                        getvalid = 1;
                    }
                    else
                    {
                        thisCycleInvalid.push_back(tb.idx);
                    }
                }
            }
            thisCycleInvalid.clear();
            ScheduleNode = TempScheduleNode;
        }

        for (int j = 0; j < scheduled_list[0].size(); j++)
        {
            cout << "control step";
            cout << setw(3) << j + 1 << ":";
            for (int d = 0; d < N; d++)
            {
                if (scheduled_list[d][j] != -1)
                    cout << ' ' << setw(3) << scheduled_list[d][j];
            }
            cout << endl;
        }
        return;
    }

  for(int i=0;i<vertex;i++)
{
    for(int j=0;j<node[i].parent.size();j++)
    {
        node[i].parent_b.set(node[i].parent[j],1)
    }
}

   bitset<vertex> task_exe;
   task_exe.reset();

   //hardware object pointer existing i.e hw
   //cluster id passed
   //

   ULL currenttime[N]={0};
    void run(EventType t)
    {





        for(int j=0;j<scheduled_list[0].size();j++)
        {
            for(int i=0;i<N;i++)
            {
                hw->clusters[id].cores[i].currenttime[i]=node[scheduled_list[i][j]].pt+currenttime[i];
                //hardware delay



            }

        }

    }
};

class LocalScheduler:publicRunableObj
{
    ULL array currenttime
    void run(EventType E)
    {
        if(E==Task_END)
            Task_finish_handler();      //start computing hardware delay
        else if(E==COMM_END)
            COMM_finish_handler();      //start the scheduling of task
    }

    COMM_finish_handler()
    {
        if()





    }

};


class GlobalSched
{
    EventSimulator *msim;

  GlobalSched(EventSimulator s,Hardware hw)   //ready with real object simulator and hardware
  {
      msim=&s;
  }


};
    EventSimulator *msim;
    GlobalSched(EventSimulator* s,Hardware h)
    {
        msim=s;
    }

    AssignResources(ProcUtilTable)
    {


    }

};


class Hardware
{
    vector<cluster> clusters;

};


class cluster
{
    unsigned int id;
    vector<core> mcore;
};

class core
{
    ULL Starttime , currenttime;
    int idx;     //index of processor
    bool state;         //either free pr busy
    float temp,freq;    //current temp and frequency of core

};






int main()
{

    app_launcher app_launcher1;
    scheduler scheduler1;


    if (app_launcher1.task_graph_input()!= 0)
    {
        cout << "Unable to preHandle\n\n";
        system("pause");
        return 0;
    }
    scheduler1.asap(node);
    scheduler1.alap(node);

    cout << "Mobility Window Result:\n" << "        ASAP | ALAP\n";

    for (int i = 0; i < vertex; i++)
    {
        cout << "node" << setw(3) << i << ":";
        cout << setw(4) << node[i].soon << " | " << setw(2) << node[i].late << endl;
    }

    scheduler1.listScheduling(node);
    system("pause");
    return 0;

}
