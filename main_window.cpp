#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QDebug>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)

{
    // UStawianie okna aplikacji
    ui->setupUi(this);
    //ustawianie polączenia miedzy kliknięciem na NOWA_GRA a metodą odświeżania/ustawienia początkowego
    //szachownicy
    QObject::connect(ui->actionNowa_Gra, SIGNAL(triggered()), this, SLOT(clearAll()));
    //LAyout służy do rozmieszczenia PRzycisków(w tym przypadku pół szachownicy)
    controlsLayout = new QGridLayout;
    //ustalanie żeby przyciski były nie miały przerwy między sobą
    controlsLayout->setHorizontalSpacing(0);
    controlsLayout->setSizeConstraint(QGridLayout::SetNoConstraint);
    controlsLayout->setVerticalSpacing(0);

    //obiekt służący do łączenia wszystkich sygnałów płynących z przycisów/pól

    signalMapper = new QSignalMapper(this);
    sizePolicy= QSizePolicy(QSizePolicy::Preferred, QSizePolicy::Preferred);
    ui->centralWidget->setLayout(controlsLayout);


    for(int i=0;i<8;i++)
    {

        for(int j=0;j<8;j++)
        {
                    QString coordinate = QString::number(i)+ QString::number(j);
                    QPushButton *button = new QPushButton();

                    connect(button, SIGNAL(clicked()), signalMapper, SLOT(map()));
                    signalMapper->setMapping(button, coordinate);
                    buttons[coordinate] = button;
                    buttons.value(coordinate)->setSizePolicy(sizePolicy);
                   buttons.value(coordinate)->move(40*j, 40*i);
                   controlsLayout->addWidget(button, i, j);
                }
     }
    clearAll();

    // Ogólnie w QT wygląda to tak że żeby doszło do jakiejś akcji typu zaznaczenia pionka podczas
    // gry to QT używa metody connect(), gdize pierwszy arguemnt to źródło z którego  idzie dany sygnał,
    //czyli jak się np. kliknęło na przycisk to on jest źródłem, natomiast drugi argument to
    //jaki obiekt ma przechycić tą informacje, a trzeci to jaki to jest typ akcji, w tym przypadku
    //jest to clicked czyli klinięcie, mogłobybyć też przeciągnięcie czegoś,itd. są różne akcje
    //Poniżej mamy jako źródło signaMapper, to dlatego że przechowuje on wszystkie przyciski powiązane
    //ze swoimi nazwami coordinate, i w zależności od tego który przycisk jest wciśnięty to idzie różna informaja
    // od przycisku (informacją jest nazwa przycisku)
    connect(signalMapper, SIGNAL(mapped(QString)), this, SIGNAL(clicked(QString)));
    connect(this, SIGNAL(clicked(QString)),this, SLOT(action(QString)));


}

//metoda ustawia szachownice tak jak to powinno na poczatku wyglądać

void MainWindow::ustaw_poczatek()
{
// wypelnienie zerami
    ustawienie = new pionki*[8];

    //tworzymy tablice 8x8
    for (int i = 0; i < 8; i++)
    {
            ustawienie[i] =  new pionki[8];
            for( int j = 0;j < 8; j++)
            {                
                //przyporządkowuje każdemu polu wartość brak,
                assignPoint(i,j,brak);
            }
    }

    // następnie tylko wybranym polom ustawia odpowiednie pionki tak jak to powinno wyglądać na początku gry

    assignPoint(0,0,wieza_1);
    assignPoint(0,7,wieza_1);
    assignPoint(7,0,wieza_2);
    assignPoint(7,7,wieza_2);

    assignPoint(0,1,goniec_1);
    assignPoint(0,6,goniec_1);
    assignPoint(7,1,goniec_2);
    assignPoint(7,6,goniec_2);

    for(int i=0;i<8;i++)
    assignPoint(1,i,zwykly_1);
    for(int i=0;i<8;i++)
    assignPoint(6,i,zwykly_2);

    assignPoint(0,2,skoczek_1);
    assignPoint(0,5,skoczek_1);
    assignPoint(7,2,skoczek_2);
    assignPoint(7,5,skoczek_2);

    assignPoint(0,3,krol_1);
    assignPoint(7,3,krol_2);
    assignPoint(0,4,krolowa_1);
    assignPoint(7,4,krolowa_2);


}


// metoda zamienia Nazwę przycisku/pola na współrzędne liczbowe które wykorzystane będą żęby odnieść
//się do tablicy ustawienie
int* MainWindow::getCoordinateFromButton(QString _name)
{

    int* coordinate = new int[2];
    coordinate[0] =_name.at(0).digitValue();
    coordinate[1] =_name.at(1).digitValue();

    return coordinate;
}


//przyporządkowuje danego pionka polu określonemu przez x,y
void MainWindow::assignPoint(int x, int y, pionki pionek)
{
    //tutaj ustawiany jest dany pionek na współrzędne w tablicy
      ustawienie[x][y] = pionek;
      //w tym przypadku zamieniami współrzędne liczbowe na STring, żeby można było odnieść się do
      //przycisku określonego przez String tych współrzędnych( Bo jak wiemy każdy przycisk,
      //jest określony przez nazwe współrzędnych)
      assignIcon(pionek, buttons.value(getCoordinateFromEnum(x,y)));

}

//metoda która  zależnośći od przyjętej nazwy pionka i określonego przez * button, przycisku,
//przyporzadkowuje/ustawia temu przyciskowi ikone z wybranych obrazków znajdujących się w Resources
void MainWindow::assignIcon(pionki pionek, QPushButton* button)
{
    if (pionek!=brak)
    {

        QString file_name = ":/Images/images/"+QString::number(pionek)+".png";
        button->setIcon(QIcon(file_name));
        button->setIconSize(QSize(42,42));

    }
    //jeżeli pole jest puste nie ma żadnej ikony
    else
         button->setIcon(QIcon(""));
    //zawsze po zmianie koloru,obrazka, rozmiarów jakiegokolwiek komponentu, który narysujemy,
    // gdzie w tym przypadku są to przyciski, trzeba wywołać metode repaint(), żeby zmiany były zauważalne
    button->repaint();
}


// metoda która przyjmuje wspołrzędne x,y bo int* tak na prawde jest tablicą dwuelementową,
//sprawdza ona ogólnie jakie jest dane pole, czy jest dostępne do ruchu itd.
avaiable MainWindow::spotIsAvaiable(int* wsp)
{
    //najpierws sprawdzamy czy w ogóle nie wychodzi poza szachownice
    if(wsp[0]<8 && wsp[0]>= 0 && wsp[1]<8 && wsp[1]>=0)
    {
        //sprawdzamy czy nazwa pionka w tym polu nie należy do nikogo
        if (GetName(wsp)==brak)
            return WOLNE;
        //w przeciwnym przypadku sprawdza czy należy do obecnego gracza
        else if(!BelongToPlayer(wsp, GetCurrentPlayer()))
            //w przeciwnym przypadku oznacza że nie zależy, czyli że jest to pole przeciwnego gracza
            return WROGIE;
        else
            return NIEDOZWOLONE;
    }
    else return NIEDOZWOLONE;





}
// metoda sprawdza czy pole o współrzędnych wsp należy do gracza "Gracz"
//jeżeli tak to zwraca True
bool MainWindow::BelongToPlayer(int* wsp, gracz Gracz)
{
    pionki pionek = GetName(wsp);
    if (Gracz == GRACZ1)
       {
        if(pionek == zwykly_1 || pionek == goniec_1 || pionek == skoczek_1 || pionek == wieza_1
               || pionek == krolowa_1 || pionek == krol_1)
            return true;
        }
    if (Gracz == GRACZ2)
       {
        if(pionek == zwykly_2 || pionek == goniec_2 || pionek == skoczek_2 || pionek == wieza_2
               || pionek == krolowa_2 || pionek == krol_2)
            return true;
        }
    return false;
}

//metoda pokazujaca mozliwe ruchy w zaleznosci od tego gdzie jest pionek, choosen to nazwa buttona
//
void MainWindow::showPossibleWays(QString choosen, gracz Gracz)
{

    // zmienna wspolrzednych przycisku, ktorym wybierac się bedzie ruch
    int* wsp = getCoordinateFromButton(choosen);

    // sprawdza czy pionek nalezy do danego gracza
    if (BelongToPlayer(wsp, Gracz))
    {
      GeneratePotentialMoves(wsp, Gracz);
        ChangeBorders(PossibleWays, "#a36218");
        ChangeBorders(PossibleAttacks, "#992812");
    }



}
//zamienia dwie liczby na tablice dwu elementową
int* MainWindow::ConvertToArray(int x,int y)
{
    int *temp = new int[2];
    temp[0]=x;
    temp[1]=y;
    return temp;
}

//metoda która dodaje dane pole do PossibleWays, czyli wektora(rodzaj tablicy), jeżeli
//dane pole jest wolne, jezeli jest WROGIE to dodaje to pole do możliwych ataków
void MainWindow::setStateOfField(int * move)
{
    avaiable field_state = spotIsAvaiable(move);
        if(field_state == WOLNE)
            //zamieniamy wartości współrzędnych ruchu na Sring tych współrzednych,
            //bo PossibleWays chcemy żeby przechowywało nazwy przycisków
            PossibleWays.append(getCoordinateFromEnum(move[0],move[1]));
        else if (field_state == WROGIE)
        {
            PossibleAttacks.append(getCoordinateFromEnum(move[0],move[1]));
        }
}

//metoda przyjmuje współrzędne wybranego pionka(na którego się kliknęło w tybie WYBOR_PIONKA, tworzone są wszystkie
//możliwe ruchy w zależności od nazwy pionka
void MainWindow::GeneratePotentialMoves(int* coordinate, gracz kierunek)
{
    pionki name = GetName(coordinate);

    if(name == zwykly_1 || name == zwykly_2)
    {
        //kierunek to tak na prawde nazwa gracza ale oznacza ona w którą strone może poruszać się pionek,
        //jako że GRACZ1 to wartość 1, a GRACZ2 to -1
        SpawnMoves(coordinate, kierunek);

    }
    else if (name == goniec_1 || name == goniec_2)
    {
        KnightMoves(coordinate);
    }
    //skoczek
    else if (name ==skoczek_1 || name == skoczek_2)
    {
       BishopMoves(coordinate);
    }
    else if (name ==wieza_1 || name == wieza_2)
    {

       CastelMoves(coordinate);
    }
    else if (name ==krol_1 || name == krol_2)
    {
       KingMoves(coordinate);
    }
    else if (name ==krolowa_1 || name == krolowa_2)
    {
        QueenMoves(coordinate);
    }
}

//każda z metod *Moves(...) polega na tym że przyjmuje współrzędne klikniętego Pola, podczas wyboru pionka
// (Tryb WYBOR_PIONKA), i względem tych współrzędnych sprawdza możliwe pola, w których może poruszyć się
//dany pionek.


//w przypadku wzywklego pionka pionek ten może poruszać się tylko do przodu,
//jezeli jest to GRACZ1 to w dół, GRACZ w góre, stąd +kierunek, bo jak będzie gracz2, to kierunkiem będzie
//-1 czyli w góre
void MainWindow::SpawnMoves(int * coordinate, gracz kierunek)
{
    //podobnie jak dla reczty rodzajów pionków, tworzona jest tymczasowe tablica dwuelementowa
    //która przechowuje współrzędne które są przesunięte o daną wartość w zaleznośći jaki to jest pionek
    //zwykły pionek porusza się o jedno pole

    int* move;
    move = new int[2];
    move[0] = coordinate[0]+kierunek;
    move[1] = coordinate[1];

    //sprawdzamy teraz czy to pole jest w ogóle dostępne w zależność od tego jakie jest ustawienie na
    //szachownicy, bo np. może być blokowane przez własnego pionka, lub wrogiego
    avaiable field_state = spotIsAvaiable(move);
        if(field_state == WOLNE)
            PossibleWays.append(getCoordinateFromEnum(move[0],move[1]));

    //reszta możliwych ruchów, ale w przypadku zwykłego pionka, tylko dwa po skosie,
        //bo pionek może tylko po skosie bić, każdy z tych ruchów sprawdzany jest metodą
        //spotIsAvaiable(), żeby sprawdzić czy jest możliwy

    //ruch po skosie w lewo
    move[0] = coordinate[0]+kierunek;
    move[1] = coordinate[1]+1;
    field_state = spotIsAvaiable(move);
    if (field_state == WROGIE)
    {
        PossibleAttacks.append(getCoordinateFromEnum(move[0],move[1]));
    }

    //ruch po skosie w prawo
    move[0] = coordinate[0]+kierunek;
    move[1] = coordinate[1]-1;
    field_state = spotIsAvaiable(move);
    if (field_state == WROGIE)
    {
        PossibleAttacks.append(getCoordinateFromEnum(move[0],move[1]));
    }
}

//ruchy króla, w odróżnieniu od pionka po utworzeniu move, uzywamy tutaj metody setOfField,
//która jest opisana wyżej
//liczba tych przypisań move = ... zależy od figury,
void MainWindow::KnightMoves(int* coordinate)
{
    int* move;
    move = new int[2]; move[0] = coordinate[0]+2; move[1] = coordinate[1]+1;setStateOfField(move);
    move = new int[2]; move[0] = coordinate[0]+2; move[1] = coordinate[1]-1;setStateOfField(move);
    move = new int[2]; move[0] = coordinate[0]+1; move[1] = coordinate[1]+2;setStateOfField(move);
    move = new int[2]; move[0] = coordinate[0]+1; move[1] = coordinate[1]-2;setStateOfField(move);
    move = new int[2]; move[0] = coordinate[0]-2; move[1] = coordinate[1]+1;setStateOfField(move);
    move = new int[2]; move[0] = coordinate[0]-2; move[1] = coordinate[1]-1;setStateOfField(move);
    move = new int[2]; move[0] = coordinate[0]-1; move[1] = coordinate[1]+2;setStateOfField(move);
    move = new int[2]; move[0] = coordinate[0]-1; move[1] = coordinate[1]-2;setStateOfField(move);
}
//królowa tak naprawdę składa się z ruchów wieży, czyli dowolnych poziomych lub pionowych,
//i dowolnych skośnych
void MainWindow::QueenMoves(int* coordinate)
{
    CastelMoves(coordinate);
    BishopMoves(coordinate);


}
//ruch wieże jest podzielony na 4 pętle po 8, ponieważ w najlepszym przypadku,
//zakładając że szachownica jest pusta i w rogu jest wieża, może ona się ruszyć aż o 8 ruchów,
//trzeba wszystko dopuścić,
//Każda pętla oznacza różna kierunki, LEWO,PRAWO,GóRa, DÓŁ, gdzie jedna pętla np. LEWO
// sprawdza możliwe kolejne pole, np. o jedno w LEWO, załóżmy żę jest dostępne
//to idziemy dalej, o 2 w LEWO, też jest dostępne, ale 3 np. już nie, to wtedy kończyły sprawdzanie
//kolejnych pól w lewo, bo wieża nie może przeskoczyć pionka, i przechodzimy do ruchów w PRAWO, GÓRA i DÓŁ
// w ten sam sposób
void MainWindow::CastelMoves(int* coordinate)
{
    int* move;
    for(int i=1;i<8;i++)
    {
        int *wsp = ConvertToArray(coordinate[0]+i,coordinate[1]);
        if(spotIsAvaiable(wsp)==NIEDOZWOLONE){ break;}
        else
        {
           move = new int[2]; move[0] = coordinate[0]+i; move[1] = coordinate[1];setStateOfField(move);
           if (spotIsAvaiable(wsp)==WROGIE)
           break;
        }
    }

    for(int i=1;i<8;i++)
    {
        int *wsp = ConvertToArray(coordinate[0]-i,coordinate[1]);
        if(spotIsAvaiable(wsp)==NIEDOZWOLONE){ break;}
        else
        {
           move = new int[2]; move[0] = coordinate[0]-i; move[1] = coordinate[1];setStateOfField(move);
           if (spotIsAvaiable(wsp)==WROGIE)
           break;
        }
    }
    for(int i=1;i<8;i++)
    {
        int *wsp = ConvertToArray(coordinate[0],coordinate[1]+i);
        if(spotIsAvaiable(wsp)==NIEDOZWOLONE){ break;}
        else
        {
           move = new int[2]; move[0] = coordinate[0]; move[1] = coordinate[1]+i;setStateOfField(move);
           if (spotIsAvaiable(wsp)==WROGIE)
           break;
        }
    }
    for(int i=1;i<8;i++)
    {
        int *wsp = ConvertToArray(coordinate[0],coordinate[1]-i);
        if(spotIsAvaiable(wsp)==NIEDOZWOLONE){ break;}
        else
        {
           move = new int[2]; move[0] = coordinate[0]; move[1] = coordinate[1]-i;setStateOfField(move);
           if (spotIsAvaiable(wsp)==WROGIE)
           break;
        }
    }

}
//podobnie jak wieża tylko że po skosie
void MainWindow::BishopMoves(int* coordinate)
{
    int* move;

    for(int i=1;i<8;i++)
    {
        int *wsp = ConvertToArray(coordinate[0]+i,coordinate[1]+i);
        if(spotIsAvaiable(wsp)==NIEDOZWOLONE){ break;}
        else
        {
           move = new int[2]; move[0] = coordinate[0]+i; move[1] = coordinate[1]+i;setStateOfField(move);
           if (spotIsAvaiable(wsp)==WROGIE)
           break;
        }
    }

    for(int i=1;i<8;i++)
    {
        int *wsp = ConvertToArray(coordinate[0]+i,coordinate[1]-i);
        if(spotIsAvaiable(wsp)==NIEDOZWOLONE){ break;}
        else
        {
           move = new int[2]; move[0] = coordinate[0]+i; move[1] = coordinate[1]-i;setStateOfField(move);
           if (spotIsAvaiable(wsp)==WROGIE)
           break;
        }
    }
    for(int i=1;i<8;i++)
    {
        int *wsp = ConvertToArray(coordinate[0]-i,coordinate[1]-i);
        if(spotIsAvaiable(wsp)==NIEDOZWOLONE){ break;}
        else
        {
           move = new int[2]; move[0] = coordinate[0]-i; move[1] = coordinate[1]-i;setStateOfField(move);
           if (spotIsAvaiable(wsp)==WROGIE)
           break;
        }
    }
    for(int i=1;i<8;i++)
    {
        int *wsp = ConvertToArray(coordinate[0]-i,coordinate[1]+i);
        if(spotIsAvaiable(wsp)==NIEDOZWOLONE){ break;}
        else
        {
           move = new int[2]; move[0] = coordinate[0]-i; move[1] = coordinate[1]+i;setStateOfField(move);
           if (spotIsAvaiable(wsp)==WROGIE)
           break;
        }
    }
}

void MainWindow::KingMoves(int* coordinate)
{
    int* move;
    move = new int[2]; move[0] = coordinate[0]+1; move[1] = coordinate[1];setStateOfField(move);
    move = new int[2]; move[0] = coordinate[0]+1; move[1] = coordinate[1]+1;setStateOfField(move);
    move = new int[2]; move[0] = coordinate[0]+1; move[1] = coordinate[1]-1;setStateOfField(move);
    move = new int[2]; move[0] = coordinate[0]-1; move[1] = coordinate[1];setStateOfField(move);
    move = new int[2]; move[0] = coordinate[0]-1; move[1] = coordinate[1]+1;setStateOfField(move);
    move = new int[2]; move[0] = coordinate[0]-1; move[1] = coordinate[1]-1;setStateOfField(move);
    move = new int[2]; move[0] = coordinate[0]; move[1] = coordinate[1]-1;setStateOfField(move);
    move = new int[2]; move[0] = coordinate[0]; move[1] = coordinate[1]+1;setStateOfField(move);
}
//metoda zmienia kolory przycisków które znajduja się w wektorze merked_buttons,
//są to albo wektory PossibleWays, albo PossibleAttacks, czyli jeżeli klinięmy na pionek
//i będzie on miał jakieś możliwe ruchy, to pola te zostaną pomalowan na dany kolor,
//tutaj znowu musi być repaint(), żeby była widoczna zmiana
void MainWindow::ChangeBorders(QVector<QString> marked_buttons, QString color)
{
    QVectorIterator<QString> i(marked_buttons);
    while (i.hasNext())
    {
        QPushButton *temp = buttons.value(i.next());
        temp->setStyleSheet("background-color: "+color);
        temp->repaint();

    }
}

//sprawdza ta metoda czy przycisk o danej nazwie znajduje się w wektorze danego Ways (Possible/Attack)
// jezeli tak to zwraca TRUE, i w metodzie action(), przechodzi do danego trybu, patrz linijka 534
bool MainWindow::checkIfAllow(QString button_name, QVector<QString> Possible)
{

        QVectorIterator<QString> i(Possible);
        while (i.hasNext())
        {
            if(i.next()==button_name)
            {
                GoalStep = button_name;
                return true;
            }
        }

    return false;
}

//zmienia aktualnego gracza na nastepnego
void MainWindow::changePlayer()
{
    if (Gracz == GRACZ2)
        Gracz = GRACZ1;
    else
        Gracz = GRACZ2;
}


//NAJWAŻNIEJSZA METODA
//odpalaa się za każdym razem jak kliknięmy jakiś guzik
//odpowiada za całą dynamikę gry
void MainWindow::action(QString button_name)
    {

    //czyści wszystkie guziki na domyślny kolor, żeby zniknęły zaznaczenia możliwego ruchu czy ataku
        colourAllButton();

        //tutaj zwróci tru tylko gdyb jakieś pola będą w POssibleAttacks, czyli najpierw trzeba wybrać
        //pionka, pojawią się dostępne ruchy i ataki, jezeli teraz klikniemy na któryś z możliwych
        //do ataku to metoda poniższa zwróci TRUE, a stanie się tak dlatego że nazwa klikniętego
        //przycisku który atakujemy będzie się znajdować w wektorze nazw pół możliwych do ataku
        if (checkIfAllow(button_name, PossibleAttacks))
            Tryb = WYBOR_ATAKU;
        //to samo tylko jeśli pole jest w wektorze PossibleWays
        else if (checkIfAllow(button_name, PossibleWays))
            Tryb = WYBOR_RUCHU;
        //jeżeli nic z tego to tryb Wyboru pionka, czyli jezeli klikniemy na pole które nie jest
        //ani w PossibleWays ani PossibleAttacks
        else
            Tryb = WYBOR_PIONKA;

        //tryb ten jest domyślny na początku gry, czyli zanim coś klikniemy
        //jesteśmy odrazu w trybie wyboru
       if(Tryb == WYBOR_PIONKA)
       {
           PossibleWays.clear();
           PossibleAttacks.clear();
           //po kliknięciu danego pionka pokazuje możliwe ruchy opisane już w metodzie
           showPossibleWays(button_name, Gracz);
           //zmienna przechowujące nazwe pionka na który kliknęliśmy , przydaje się podczas,
           //przesunięcia pionka, żeby program pamiętał z jakiego miejsca się ruszył
           FromStep = button_name;
         }
       else if(Tryb == WYBOR_RUCHU)
       {
           //czyścimy wektora zeby nie były pokolorwane dane pola w następnym ruchu
           PossibleWays.clear();
           PossibleAttacks.clear();
           //przesuwamy pionek, z pola w którym został kliknięty do pola do którego wybraliśmy
           // z możliwych (PossibleWays musi zawierać to pole)
           moveFigure(FromStep, GoalStep);
           Tryb = WYBOR_PIONKA;

           //koniec ruchu, zmiana gracza

           changePlayer();

       }
       else if(Tryb == WYBOR_ATAKU)
       {
           //podobnie jak w WYBOR_RUCHU tylko usuwa pionka
           moveFigure(FromStep, GoalStep);
           Tryb = WYBOR_PIONKA;
           PossibleWays.clear();
           PossibleAttacks.clear();
           changePlayer();
       }


    }

//metoda przyjmuje nazwe pola z którego wykonujemy ruch, i nazwe pola do którego wykonujemy ruch
void MainWindow::moveFigure(QString from, QString goal)
{
    //wsporzedne pionka ktory jest przenoszony
    int* temp = getCoordinateFromButton(from);
    //przechwycenie nazwy pionka
     pionki temp_val = GetName(temp);

    //usuniecie z pierwszego pola pionka
    assignPoint(temp[0],temp[1],brak);
    //dodanie go do wskazanego pola
   temp = getCoordinateFromButton(goal);
   assignPoint(temp[0],temp[1],temp_val);
}

//zwraca prznależność Pionka na podstawie podanych współrzędnyh
gracz MainWindow::getPlayer(int x, int y)
{
    //wartość <6 oznacza pionki gracza 1, dlatego że nazwy pionków są powiązane z numerami, patrz enum pionki
    if(ustawienie[x][y]<6)
        return GRACZ1;
    else if (ustawienie[x][y]>6)
        return GRACZ1;
    else
        return NICZYJE;
}

QString MainWindow::getCoordinateFromEnum(int i, int j)
{
    QString button_code  = QString::number(i)+QString::number(j);
    return button_code;
}

pionki MainWindow::GetName(int* xy)
{
    int* temp = xy;

    return ustawienie[temp[0]][temp[1]];
}


// metoda kolorujaca przyciski

void MainWindow::colourAllButton()
{
    for(int i=0;i<8;i++)
    {
        for(int j=0;j<8;j++)
        {
            QPushButton *temp = buttons.value(QString::number(i)+QString::number(j));
            if((j+i)%2==0)
                temp->setStyleSheet( "background-color: #efb370");
            else
                temp->setStyleSheet( "background-color: #f7e1c8");
        }
    }

}

void MainWindow::clearAll()
{
    Tryb = WYBOR_PIONKA;
    Gracz = GRACZ1;
    ustaw_poczatek();
    colourAllButton();
//    assignIcons(buttons,ustawienie);

}

gracz MainWindow::GetCurrentPlayer()
{
    return Gracz;
}

MainWindow::~MainWindow()
{
    delete ui;
}

