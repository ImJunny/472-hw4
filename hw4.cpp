//----------------------------------------------------------
// Name: John Nguyen
// Email: jnn5163@psu.edu
// Class: CMPSC 472-001, Fall 2024
//----------------------------------------------------------

//SIMPLE variables to indicate non-hayride actions
int seated = 0;
int has_eaten = 0;
semaphore cout_mutex;
semaphore all_seated;
semaphore seat_mutex;
semaphore turkey_carved;
semaphore eat_mutex;
semaphore all_eaten;
semaphore excused;

// WAITING variables
int r_waiting[6]; // store ids of relatives waiting
int c_waiting[7]; // store ids of children waiting
int m_waiting = 0;
int l_waiting = 0;
int r_index = 0; // increment next available index to store ids in r_waiting
int c_index = 0; // increment next available index to store ids in c_waiting
semaphore r_waiting_mutex;
semaphore c_waiting_mutex;

// HAYRIDE DONE variables
semaphore r_done[6]; // signal/wait for semaphore based on relative ids; each id has index [id-1]
semaphore c_done[7]; // signal/wait for semaphore based on children ids; each id has index [id-1]
semaphore m_done;
semaphore l_done;

// delay function to simulate time between actions
void Delay(void){
    int i;
    int DelayTime;
    DelayTime = random(100);
    for(i=0; i<DelayTime; i++);
}

// MOTHER process
void Mother(){
    while(1){
        wait(cout_mutex);
        cout<<"\t\tMother is making meal."<<endl;
        signal(cout_mutex);
        Delay();   

        wait(cout_mutex);
        cout<<"\t\tMother is waiting for hayride."<<endl;
        signal(cout_mutex);
        m_waiting = 1;
        Delay();  

        wait(m_done);

        wait(cout_mutex);
        cout<<"\t\tMother is sitting at table."<<endl;
        signal(cout_mutex);  
        Delay(); 

        // increment number of seated; signal all_seated if necessary
        wait(seat_mutex);
        seated = seated + 1;
        signal(seat_mutex);
        if (seated==16) signal(all_seated);
        
        wait(turkey_carved);
        wait(cout_mutex);
        cout<<"\t\tMother is eating."<<endl;
        signal(cout_mutex);
        Delay(); 

        // increment number of has_eaten; signal all_eaten if necessary
        wait(eat_mutex);
        has_eaten = has_eaten + 1;
        signal(eat_mutex);
        if (has_eaten==16) signal(all_eaten);

        wait(excused);
        wait(cout_mutex);
        cout<<"\t\tMother goes to read her book."<<endl;
        signal(cout_mutex);
        Delay(); 

        break;
    }

}

int fr_index = 0;
int fc_index = 0;
int hayrides = 0;
// helper function to signal c_done and r_done for children/relatives to proceed with tasks
// and update number of hayrides
void signalDone(int c, int r){
    int i;

    int j=0;
    for (i=0; i<c; i++){
        signal(c_done[c_waiting[fc_index + j]-1]);
        j = j+1;
    }
    j=0;
    for (i=0; i<r; i++){
        signal(r_done[r_waiting[fr_index + j]-1]);
        j = j+1;
    }

    fc_index = fc_index + c;
    fr_index = fr_index + r;
    hayrides = hayrides + c + r;
}

// FATHER process
void Father(){
    int i;
    while(1){
        wait(cout_mutex);
        cout<<"\t\t\tFather is ready to give hayrides."<<endl;
        signal(cout_mutex);
        Delay(); 

        while(hayrides<15){
            // loop and first checks whether index of array is valid, then signal semaphores and update indices
            // 3 children
            if (fc_index+2<7){
                if(c_waiting[fc_index]>0 && c_waiting[fc_index+1]>0 && c_waiting[fc_index+2]>0){
                    wait(cout_mutex);
                    cout<<"\t\t\tFather gives hayride to Child "<<c_waiting[fc_index]<<", ";
                    cout<<"Child "<<c_waiting[fc_index+1]<<", and Child "<<c_waiting[fc_index+2]<<"."<<endl;
                    signal(cout_mutex);
                    signalDone(3,0);
                }
            // 2 children, 1 relative
            }else if (fc_index+1<7 && fr_index<6){
                if(c_waiting[fc_index]>0 && c_waiting[fc_index+1]>0 && r_waiting[fr_index]>0){
                    wait(cout_mutex);
                    cout<<"\t\t\tFather gives hayride to Child "<<c_waiting[fc_index]<<", ";
                    cout<<"Child "<<c_waiting[fc_index+1]<<", and Relative "<<r_waiting[fr_index]<<"."<<endl;
                    signal(cout_mutex);
                    signalDone(2,1);
                }
            }            
            // Mother, Child 8, and Child 
            else if (fc_index<7 && m_waiting==1 && l_waiting==1){
                if (c_waiting[fc_index]>0){
                    wait(cout_mutex);
                    cout<<"\t\t\tFather gives hayride to ";
                    cout<<"Mother, Child 8, and Child "<<c_waiting[fc_index]<<"."<<endl;
                    signal(cout_mutex);
                    signal(m_done);
                    signal(l_done);
                    signalDone(1,0);
                    hayrides = hayrides + 2;
                }
            // 2 relatives
            }else if (fr_index+1<6){
                if (r_waiting[fr_index]>0 && r_waiting[fr_index+1]>0){
                    wait(cout_mutex);
                    cout<<"\t\t\tFather gives hayride to ";
                    cout<<"Relative "<<r_waiting[fr_index]<<" and Relative "<<r_waiting[fr_index+1]<<"."<<endl;
                    signal(cout_mutex);
                    signalDone(0,2);
                }
            }else if (hayrides==13){
                // Mother, Child 8
                if (m_waiting==1 && l_waiting==1){
                    wait(cout_mutex);
                    cout<<"\t\t\tFather gives hayride to ";
                    cout<<"Mother and Child 8."<<endl;
                    signal(cout_mutex);
                    signal(m_done);
                    signal(l_done);
                    fc_index = fc_index + 1;
                    hayrides = hayrides + 2;
                // 2 children
                }else if (fc_index+1<7){
                    if(c_waiting[fc_index]>0 && c_waiting[fc_index+1]>0){
                        wait(cout_mutex);
                        cout<<"\t\t\tFather gives hayride to ";
                        cout<<"Child "<<c_waiting[fc_index]<<" and Child"<<c_waiting[fc_index]<<"."<<endl;
                        signal(cout_mutex);
                        signalDone(2,0);  
                    }
                // 1 child, 1 relative  
                }else if (fc_index<7 && fr_index<6){
                    if(c_waiting[fc_index]>0 && r_waiting[fr_index]>0){
                        wait(cout_mutex);
                        cout<<"\t\t\tFather gives hayride to ";
                        cout<<"Child "<<c_waiting[fc_index]<<" and Relative "<<r_waiting[fr_index]<<"."<<endl;
                        signal(cout_mutex);
                        signalDone(1,1);                      
                    }
                }
            }else if (hayrides==14){
                // 1 child
                if (fc_index<7){
                    if(c_waiting[fc_index]>0){
                        wait(cout_mutex);
                        cout<<"\t\t\tFather gives hayride to ";
                        cout<<"Child"<<c_waiting[fc_index]<<"."<<endl;
                        signal(cout_mutex);
                        signalDone(1,0);                      
                    }
                }
                // 1 relative
                if (fr_index<6){
                    if(r_waiting[fr_index]>0){
                        wait(cout_mutex);
                        cout<<"\t\t\tFather gives hayride to ";
                        cout<<"Relative"<<r_waiting[fr_index]<<"."<<endl;
                        signal(cout_mutex);
                        signalDone(0,1);
                    }
                }
            }
        }

        wait(cout_mutex);
        cout<<"\t\t\tFather is sitting at table."<<endl;
        signal(cout_mutex);
        Delay(); 

        // increment number of seated; signal all_seated if necessary
        wait(seat_mutex);
        seated = seated + 1;
        signal(seat_mutex);
        if (seated==16) signal(all_seated);
        
        wait(all_seated);
        wait(cout_mutex);
        cout<<"\t\t\tFather carves turkey."<<endl;
        signal(cout_mutex);
        Delay(); 

        for(i=0; i<16; i++) signal(turkey_carved);
        
        wait(turkey_carved);
        wait(cout_mutex);
        cout<<"\t\t\tFather is eating."<<endl;
        signal(cout_mutex);
        Delay(); 

        // increment number of has_eaten; signal all_eaten if necessary
        wait(eat_mutex);
        has_eaten = has_eaten + 1;
        signal(eat_mutex);
        if (has_eaten==16) signal(all_eaten);

        wait(all_eaten);
        wait(cout_mutex);
        cout<<"\t\t\tFather excuses everyone from table."<<endl;
        signal(cout_mutex);
        Delay(); 

        for(i=0; i<16; i++) signal(excused);

        wait(excused);
        wait(cout_mutex);
        cout<<"\t\t\tFather goes to watch football."<<endl;
        signal(cout_mutex);
        Delay(); 

        break;
    }
}

// CHILD process
void Child(int id){
    while(1){
        wait(cout_mutex);
        cout<<"\tChild "<<id<<" is raking leaves."<<endl;
        signal(cout_mutex);
        Delay(); 

        wait(cout_mutex);
        cout<<"\tChild "<<id<<" is waiting for hayride."<<endl;
        signal(cout_mutex);
        Delay(); 

        if (id!=8){
            wait(c_waiting_mutex);
            c_waiting[c_index] = id;
            c_index = c_index + 1;
            signal(c_waiting_mutex);
            wait(c_done[id-1]); // r_done stores semaphore at [id-1]
        }else{
            l_waiting = 1;
            wait(l_done);
        }

        wait(cout_mutex);
        cout<<"\tChild "<<id<<" is sitting at table."<<endl;
        signal(cout_mutex);
        Delay(); 

        // increment number of seated; signal all_seated if necessary
        wait(seat_mutex);
        seated = seated + 1;
        signal(seat_mutex);
        if (seated==16) signal(all_seated);
        
        wait(turkey_carved);
        wait(cout_mutex);
        cout<<"\tChild "<<id<<" is eating"<<endl;
        signal(cout_mutex);
        Delay(); 

        // increment number of has_eaten; signal all_eaten if necessary
        wait(eat_mutex);
        has_eaten = has_eaten + 1;
        signal(eat_mutex);
        if (has_eaten==16) signal(all_eaten);

        wait(excused);
        wait(cout_mutex);
        cout<<"\tChild "<<id<<" goes out to play."<<endl;
        signal(cout_mutex);
        Delay(); 

        break;
    }
}

// RELATIVE process
void Relative(int id){
    while(1){
        wait(cout_mutex);
        cout<<"Relative "<<id<<" arrives."<<endl;
        signal(cout_mutex); 
        Delay(); 

        wait(cout_mutex);
        cout<<"Relative "<<id<<" is waiting for hayride."<<endl;
        signal(cout_mutex);   
        Delay(); 

        wait(r_waiting_mutex);
        r_waiting[r_index] = id;
        r_index = r_index + 1;
        signal(r_waiting_mutex);

        wait(r_done[id-1]); // r_done stores semaphore at [id-1]

        wait(cout_mutex);
        cout<<"Relative "<<id<<" is sitting at table."<<endl;
        signal(cout_mutex);    
        Delay();   

        // increment number of seated; signal all_seated if necessary
        wait(seat_mutex);
        seated = seated + 1;
        signal(seat_mutex);
        if (seated==16) signal(all_seated);

        wait(turkey_carved);
        wait(cout_mutex);
        cout<<"Relative "<<id<<" is eating."<<endl;
        signal(cout_mutex);
        Delay(); 

        // increment number of has_eaten; signal all_eaten if necessary
        wait(eat_mutex);
        has_eaten = has_eaten + 1;
        signal(eat_mutex);
        if (has_eaten==16) signal(all_eaten);

        wait(excused);
        wait(cout_mutex);
        cout<<"Relative "<<id<<" goes home."<<endl;
        signal(cout_mutex);
        Delay(); 

        break;  
    }
}

// MAIN
main(){
    int j;
    // initialize all semaphores
    initialsem(cout_mutex, 1);
    initialsem(all_seated,0);
    initialsem(seat_mutex, 1);
    initialsem(turkey_carved, 0);
    initialsem(eat_mutex, 1);
    initialsem(all_eaten, 0);
    initialsem(excused, 0);
    initialsem(r_waiting_mutex, 1);
    initialsem(c_waiting_mutex, 1);
    initialsem(m_done, 0);
    initialsem(l_done, 0);
    for (j=0; j<6; j++) initialsem(r_done[j],0);
    for (j=0; j<7; j++) initialsem(c_done[j],0);
    cobegin{
        Mother();
        Father();
        Child(1);
        Child(2);
        Child(3);
        Child(4);
        Child(5);
        Child(6);
        Child(7);
        Child(8);
        Relative(1);
        Relative(2);
        Relative(3);
        Relative(4);
        Relative(5);
        Relative(6);
    } 
}