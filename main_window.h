#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QPushButton>
#include <QGridLayout>
#include <QSignalMapper>
#include <QMap>

enum pionki
{ zwykly_1 = 0, goniec_1 = 1, skoczek_1 = 2, wieza_1 = 3, krolowa_1 = 4, krol_1 = 5, brak = 6 ,
zwykly_2 = 7, goniec_2 = 8, skoczek_2 = 9, wieza_2 = 10, krolowa_2 = 11, krol_2 = 12};

enum tryb
{ WYBOR_PIONKA = 0, WYBOR_RUCHU = 1, WYBOR_ATAKU = 2, SZACH = 3};


enum tryb_ruchu
{ ATAK = 0, RUCH = 1};

//stan danego pola dla danego gracza
enum avaiable
{ WOLNE = 1, WROGIE = 3, NIEDOZWOLONE = 4};

enum gracz
{ GRACZ1 = 1, GRACZ2 = -1, NICZYJE = 0};

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
   //
    tryb Tryb;
    pionki** ustawienie;
    QString choosen;
    gracz Gracz;
    //zmienna przechowujaca wspolrzedne do ktorych chcemy przemiescic
    QString GoalStep;
    //zmienna przechowujaca wspolrzedne z ktorych sie przemieszczamy
    QString FromStep;
    QGridLayout *controlsLayout;
    QVector<QString> PossibleWays;
     QVector<QString> PossibleAttacks;
    QMap<QString, QPushButton*> buttons;
    QSizePolicy sizePolicy;
    explicit MainWindow(QWidget *parent = 0);
    QSignalMapper* signalMapper;


    //metody

    // ustawiamy od gory, pierwsze dwa gorne rzedy i dwa dolne,
    void ustaw_poczatek();

    //metoda zamienia string z mapowania na liczby
    int* getCoordinateFromButton(QString);

    //metoda zamienia z liczb na string do buttonow
    QString getCoordinateFromEnum(int i, int j);

    //metoda sprawdzajaca czy wspolrzedne wychodza po za tablice
    bool CheckBoundary(int x, int y);

    //metoda laduje obrazek okreslony przez pionek do odpowiedniego przycisku
    void assignIcon(pionki pionek, QPushButton*);

    //metoda rysujaca mozliwe drogi
    void showPossibleWays(QString, gracz);

    // na podstawie wsp zwraca do ktorego gracza nalezy to pole
    gracz getPlayer(int i, int j);

    //koloruje wszystkie przyciski
    void colourAllButton();
    //zamienia okrelonemu zestawowi button kolor tla
    void ChangeBorders(QVector<QString>, QString );

    //metoda sprawdzajaca czy w trybie wyboru pionka mozliwe jest dany ruch, jesli tak to tryb zmienia sie na ruch
    bool checkIfAllow(QString, QVector<QString>);

    //metoda przesuwajaca figure z jednego pola na inne na poziomie tablicy
    void moveFigure(QString, QString);

    //zmienia gracza
    void changePlayer();

    //nadaje na tablicy wartosc pionka i jednoczesnie nadaje przyciskowi odpowiednia ikone
    void assignPoint(int x, int y, pionki pionek);

    //zwraca nazwe pionka przyjmujac argument tablicy dwuwymiarowej
    pionki GetName(int*);

    //sprawdza czy dany pionek nalezy do gracza
    bool BelongToPlayer(int*, gracz);

    //metoda sprawdza czy miejsce jest dozwolone
    avaiable spotIsAvaiable(int*);

    // generuje potencjalne ruchy nie uwzgledniajac przeszkod
    void GeneratePotentialMoves(int* coordinate, gracz kierunek);

    //ustawia stan danego pola dla wybranego pionka, np. jezeli pionek nie moze go dosiegnac to nic nie bedzie
    void setStateOfField(int * move);

    //zamienia wspolrzedne na tablice dwuch elementow
    int* ConvertToArray(int x,int y);

    gracz GetCurrentPlayer();


    //ruchy pionków
    void SpawnMoves(int*,gracz);
    void KingMoves(int*);
    void KnightMoves(int*);
    void QueenMoves(int*);
    void CastelMoves(int*);
    void BishopMoves(int*);


//ostateczna tura
    //sprawdza czy jest Szach, przyjmuje argument gracza który jest wykonuje szach
    bool Check(gracz);

    ~MainWindow();

Ui::MainWindow *ui;

signals:
    void clicked(const QString);

private slots:
   void action(QString);
   //czysci wszystko
   void clearAll();
};

#endif // MAINWINDOW_H
