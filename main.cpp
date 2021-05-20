#include <bits/stdc++.h>

using namespace std;
/*
- insert
- delete min
- build
- merge
{1}
*/
template <class T>
class node
{
public:
    T value;
    node * left, *right, *parent, *child;
    // tinem pointer doar la un singur copil
    // pentru ca nu ii pierdem pe restul
    // deoarece fiecare sunt linked prin left-right circular
    int degree; // = numarul de fii directi

    node()
    {
        left = NULL;
        right = NULL;
        parent = NULL;
        child = NULL;
        degree = 0;
    }
};

template <class T>
class FibonacciHeap
{
public:
    int size;
    node<T>* pivot; // in loc sa tinem un inceput si final
    // tinem doar minimul si la parcurgere facem o "bucla"
    // si cand ne intoarcem la minim este clar ca am terminat parcurgerea

    FibonacciHeap<T>()
    {
        this -> size = 0;
        this -> pivot = NULL;
    }


     void _display(node<T>* end) // linia principala
    // end = pivot explicat mai sus
    {
         node<T>* iterator = pivot;
        if (iterator == NULL) // niciun nod in FH
            return;
        do
        {
            // cout << iterator->value << " ";
            iterator = iterator->right;
        }while(iterator  != end && iterator -> right != NULL);
        // merg din nod in nod spre dreapta
        // pana ajung iar la mini/end sau nu mai am niciun nod
    }

    void _insert(T key)
    {
        size++;
        node<T>* inserted = new node<T>; // nodul care va fi adaugat
        inserted->value = key;
        inserted->left = inserted; // il punem sa pointeze la el insusi
        inserted->right = inserted;
        // degree ramane 0 pt ca inseram un singur nod in
        // "linia principala" => nu are fii
        if (pivot == NULL) // daca minimul este null
            // atunci minimul va deveni noul nod adaugat
            // si asta este tot
        {
            // cout << "ok";
            pivot = inserted;
            // cout << pivot -> value;
            return;
        }
        // mai avem noduri in FH
        pivot->left->right = inserted; // il inseram pe locul lui mini
        // => mini se duce in dreapta lui inserted
        inserted->right = pivot;
        inserted->left = pivot->left;
        pivot->left = inserted;

        if (pivot->value < key) // actualizam maximul
            pivot = inserted;
    }

    T _getMin()
    {
         return pivot->value;
    }

    void _unparent_all(node<T>* current)
    {
        // iau o linie de noduri copii
        // si le tai legatura cu nodul parinte
        // current = nod copil
        if (current == NULL)
            return;

        node<T> * iterator = current;
        do
        {
            iterator->parent = NULL;
            iterator = iterator -> right;
        }while (iterator != current); // am taiat legatura parinte-copil
        // de pe intreaga linie pe care se afla current
    }

    void _removeCircular(node<T>* current)
    {
        if (current -> right == current) // => un singur nod
            return;

        node<T>* Leftcurrent = current ->left;
        node<T> * Rightcurrent = current -> right;

        Leftcurrent->right = Rightcurrent;
        Rightcurrent->left = Leftcurrent;
        // am scos nodul current din linia cu legaturi circulare
    }

    node<T>*  _merge(node<T>* x, node<T>* y)
    {
        if (x == NULL) return y; // nu am ce nod sa adaug
        if (y == NULL) return x; // nu am ce nod sa adaug

        if (x->value < y->value) // swap
            // x = valoarea mai mare
        {
            node<T>* aux = y;
            y = x;
            x = aux;
        }

        node<T>* rightX = x -> right;
        node<T>* leftY = y -> left;
        x->right = y; // am legat y in dreapta lui x
        y->left = x;

        rightX->left = leftY;
        leftY->right = rightX;
        // cout << "returned max val " << x->value  << " between " << x -> value << "  " << y-> value<< endl;
        return x;
    }

    void _make_child(node<T>* parinte, node<T>* newchild)
    {
        this->_removeCircular(newchild); // daca e in lista il scot
        newchild->left = newchild -> right = newchild; // fac copilul sa fie pointeze catre el
        newchild->parent = parinte;
        parinte -> child = _merge(parinte->child, newchild); // realoc pointerul de copil
        // dupa ce adaug noul copil
        parinte ->degree += 1; // i-am adaugat un fiu direct
    }


    void _consolidate()
    {
        // orice nod din FH are maxim [logN] copii
        int Dimension = (int) (log(size)/ log(2));
        //// cout << endl <<Dimension << endl
        // degree-ul maxim al nodurilor
        node<T>* p_Degree[Dimension + 1];
        // p_Degree[0] = pointer la un nod din "linia principala"
        // cu degree = 0; samd
        for (int i = 0; i < Dimension + 1; ++i)
            p_Degree[i] =  NULL;

        node<T>* iterator = pivot;
        bool break_flag = 0; // pt ca mergem cu while(true) avem nevoie de o modalitate de iesire
        int current_degree;
        do
        {

            current_degree = iterator -> degree; // gradul nodului curent
            while(p_Degree[current_degree] != NULL)
                // => am dat de 2 noduri cu acelasi grad
                // => trebuie sa le combinam
            {
                node<T> *p = p_Degree[current_degree];
                if (p == iterator)
                {
                    break_flag = 1; // inseamna ca nu mai e nevoie sa parcurgem
                    // in continuare linia
                    break; // => am dat de acelasi nod
                }

                if (iterator -> value < p -> value)
                    // swap iterator cu p ca sa am mereu in iterator
                    // nodul cu valoarea maxima
                {
                    node<T>* aux = p;
                    p = iterator;
                    iterator = aux;
                }

                this->_make_child(iterator, p);
                // iterator devine parintele lui p
                p_Degree[current_degree] = NULL; // pt ca noul nod generat are degree = current_degree + 1
                current_degree++; // ca sa "lipim" in continuare
            }

            if (break_flag) break; // repetam parcurgerile si trebuie sa iesim

            p_Degree[iterator -> degree] = iterator;
            iterator = iterator -> right;
        }while (true);

        pivot = NULL; // trebuie sa realocam pivotul
        node<T>* stop = iterator;
        do
        {
            if (pivot == NULL || iterator -> value > pivot -> value)
                pivot = iterator;
            // cout << " result " << pivot -> value << endl;
            iterator = iterator -> right;
        }while (iterator != stop);
    }

    node<T>* _deleteMin()
    {
        if (pivot == NULL) // nu am de unde sa dau delete
            return NULL;

        node<T> *aux = pivot;
        this->_unparent_all(aux->child);
        // =>  tai legatura copiilor cu pivot pt ca urmeaza la il scot
        this ->_merge(aux, aux->child);
        // leg linia de copii directi de "linia principala"
        this -> _removeCircular(aux); // scot pivot de pe linia principala
        size --;

        if (aux == aux ->right )// => e doar el
            pivot = NULL; // pt ca aux este sters din FH
        else
        {
            pivot = aux -> right;
//            // cout << endl;
//           this->_display(pivot);
//           // cout << endl;
            this -> _consolidate();
        }
        return aux;
    }

    void mergeHeaps(FibonacciHeap& fh)
    // meld de heap-uri
    {
        pivot = this->_merge(pivot, fh.pivot);
        // adaug la linia principala nodurile de pe linia principala a celuilalt heap
        // pivot trebuie sa devina maximul dintre cele doua maxime
        // cout << pivot -> value << "\n";
        size += fh.size; // am adaugat nodurile in heap
        fh.size = 0; // celelalt heap a ramas fara noduri
        fh.pivot = NULL;
    }
};


int main()
{
    /*
    FibonacciHeap<int> f;
    f._insert(4);
    f._insert(8);
    f._insert(6);
    f._display(f.pivot);  // cout << endl;
    f._deleteMin();
    f._display(f.pivot); // cout << endl;
    f._insert(8);
    f._insert(9);
    f._insert(10);
    f._display(f.pivot); // cout << endl;
    f._deleteMin();
    f._display(f.pivot); // cout << endl;
    return 0;
    */
    ifstream fin("mergeheap.in");
    ofstream fout("mergeheap.out");
    int N, K, type, where, who;
    fin >> N >> K;
    vector<FibonacciHeap<int>> multimi;
    for (int i = 0; i < N; ++i)
        multimi.push_back(FibonacciHeap<int>());
    for (; K > 0; K--)
    {
        fin >> type;
        // cout <<"\n\n";
//        for (int i = 1; i <= N; ++i)
//            {
//                 // cout << i << "\t";
//                if (multimi[i].pivot != NULL)
//                    multimi[i]._display(multimi[i].pivot);
//                // cout << endl;
//            }
        switch(type)
        {
        case 1:
            {
                fin >> where >> who;
                // cout << "\tinserted " << who << " in " << where << endl;
                multimi[where - 1]._insert(who);
                //multimi[where]._display(multimi[where].pivot);
               // // cout << endl;
                break;
            }
        case 2:
            {
                fin >> where;
                // cout <<"pop max " << where << endl;
                fout << multimi[where - 1]._getMin() << endl;
                multimi[where - 1]._deleteMin();
                break;
            }
        case 3:
            {
                fin >> where >> who;
                // cout << "merging \n";
                // cout << where << "\t";
//                multimi[where]._display(multimi[where].pivot);
                // cout << endl;
                // cout << who << "\t";
//                 multimi[who]._display(multimi[who].pivot);
                multimi[where - 1].mergeHeaps(multimi[who - 1]);
//                multimi[where]._display(multimi[where].pivot);
                break;
            }
        }
    }

    return 0;
}
