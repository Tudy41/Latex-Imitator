#include <iostream>
#include <string>
#include <cstring>
#include <stack>
#include <algorithm>
#include <cstring>
#include <winbgim.h>
#include <graphics.h>
#include <stdio.h>
 
using namespace std;
 
string istoricformule[100];
int nrElementeIstoric = 0;
 
string formula_infixata;
string sirDeTokenuri[300];
string sirPostfixat[200];
 
int nrMembri = 0;
int nrTokenuri = 0;
int lungimeEcran, latimeEcran;
int coeficientScala = 5;
bool minusflag = false;
 
struct arbore {
    string info;
    arbore* stanga;
    arbore* unar;
    arbore* dreapta;
    int indice_nod_special=-1;
};
struct nod_special{
    string sus;
    string jos;
    bool stocat_pe_operand; ///false = pus pe operator
    int indice_pozitie;
    int tip; /// 1 pentru suma
            /// 2 pentru produs
            /// 3 pentru integrala nedefinita
            /// 4 pentru integrala definita
}arr_indici_noduri_speciale[10];
 
arbore* ArbConstruit;
 
struct Cursor{
int x;
int y;
};
 
struct setari{
    int culoare1;
    int culoare2;
    int metodaInput=1;
    bool asteapta5secunde=false; ///deoarece nu exista mod prin care programul doar sa verifice daca s-a apasat o tasta fara sa opreasca intreg programul,
                                ///se poate seta daca fereastra se va inchide dupa 5 secunde sau dupa metoda getch();
    bool afisareDirecta=true;   ///afisare directa dupa ce s-a introdus formula, fie in consola, fie in modul grafic
    bool salvareInIstoric=true;
    bool salvareFormulaInPoza=false;
    int nr_imagini=0;
}setari;
 
 
void proceduraSalvareSetari(){
    FILE* fisierSetari;
    if ((fisierSetari = fopen("setari.bin","wb")) == NULL){
            cout<<"Eroare la scrierea fisierului!"<<endl;
    }
    else {
        fwrite(&setari, sizeof(setari), 1, fisierSetari);
        cout<<"Setarile au fost salvate!"<<endl;
        fclose(fisierSetari);
    }
}
void proceduraImportareSetari(){
    FILE* fisierSetari;
    if ((fisierSetari = fopen("setari.bin","rb")) == NULL){
            cout<<"Eroare la citirea fisierului!"<<endl;
    }
    else {
        fread(&setari, sizeof(setari), 1, fisierSetari);
        cout<<"Setarile au fost importate!"<<endl;
        fclose(fisierSetari);
    }
}
void buton(int x, int y, char textdeafis[], int culoare){
    settextstyle(BOLD_FONT,HORIZ_DIR,3);///merge cu orice font
 
    int x1=x-textwidth(textdeafis)/2-15;
    int y1=y-textheight(textdeafis)/2-15;
    int x2=x+textwidth(textdeafis)/2+15;
    int y2=y+textheight(textdeafis)/2+15;
 
    setfillstyle(SOLID_FILL,culoare);
    rectangle(x1,y1,x2,y2);     ///desenez conturul dreptunghiului
 
    floodfill(x1+1,y1+2,15);    ///desenez culoarea de fundal
 
    setbkcolor(culoare);    ///in mod prestabilit, fundalul este negru pentru text, aici setez culoarea conform cerintelor.
    outtextxy(x-textwidth(textdeafis)/2, y-textheight(textdeafis)/2, textdeafis);
}
void fereastraSetari(){
    int x_buton_culoare1=100, y_buton_culoare1=70;
    int x_buton_culoare2=400, y_buton_culoare2=70;
    int x_buton_albnegru=250, y_buton_albnegru=70;
    int x_buton_metodainput=100, y_buton_metodainput=175;
    int x_buton_asteapta5sec=350, y_buton_asteapta5sec=175;
    int x_buton_afisareDirecta=250, y_buton_afisareDirecta=370;
    int x_buton_salvareInIstoric=250, y_buton_salvareInIstoric=273;
    int x_buton_salvareImagine=250, y_buton_salvareImagine=460;
    int x_buton_exit=250,y_buton_exit=550;
 
    initwindow(500, 600, "SETARI", 100, 200);
 
        setfillstyle(SOLID_FILL,9);     ///background
        floodfill(1,2,9);               ///background
 
    int culoare1;
    if(setari.culoare1==WHITE) culoare1=0;
    else culoare1=setari.culoare1;
 
    int culoare2;
    if(setari.culoare2==WHITE) culoare2=0;
    else culoare2=setari.culoare2;
 
    buton(x_buton_culoare1, y_buton_culoare1, "CULOARE1", culoare1); ///buton selectie culoare
    buton(x_buton_culoare2, y_buton_culoare2, "CULOARE2", culoare2);
 
    buton(x_buton_exit, y_buton_exit, "EXIT", RED);
 
 
    if(setari.culoare1==WHITE && setari.culoare2==BLACK) buton(x_buton_albnegru, y_buton_albnegru, "W/B", BLACK);
    else buton(x_buton_albnegru, y_buton_albnegru, "B/W", BLACK);
 
    if(setari.metodaInput==1)
        buton(x_buton_metodainput, y_buton_metodainput, "MOD GRAFIC", LIGHTRED);
    else buton(x_buton_metodainput, y_buton_metodainput, ">[CONSOLA]", BLACK);
 
    if(setari.afisareDirecta==true)
        buton(x_buton_afisareDirecta, y_buton_afisareDirecta, "AFISARE DIRECTA: DA", LIGHTGREEN);
    else buton(x_buton_afisareDirecta, y_buton_afisareDirecta, "AFISARE DIRECTA: NU", BROWN);
 
    if(setari.asteapta5secunde==true)
        buton(x_buton_asteapta5sec, y_buton_asteapta5sec, "Asteapta 5 sec: DA", LIGHTCYAN);
    else buton(x_buton_asteapta5sec, y_buton_asteapta5sec, "Asteapta 5 sec: NU", BLUE);
 
    if(setari.salvareInIstoric==true)
        buton(x_buton_salvareInIstoric, y_buton_salvareInIstoric, "ISTORIC FORMULE: DA", LIGHTGRAY);
    else buton(x_buton_salvareInIstoric, y_buton_salvareInIstoric, "ISTORIC FORMULE: NU", BLACK);
 
    if(setari.salvareFormulaInPoza==true)
        buton(x_buton_salvareImagine, y_buton_salvareImagine, "Salvare formula in imagine: DA", GREEN);
    else buton(x_buton_salvareImagine, y_buton_salvareImagine, "Salvare formula in imagine: NU", RED);
 
 
    int x,y;
    bool exit=false;
    int alb_negru=false;
 
    while(!exit){
        if(ismouseclick(WM_LBUTTONDOWN)){
            clearmouseclick(WM_LBUTTONDOWN);
            x=mousex();
            y=mousey();
 
            if(x>(x_buton_culoare1-textwidth("CULOARE1")/2-15)&&x<(x_buton_culoare1+textwidth("CULOARE1")/2+15) && y>(y_buton_culoare1-textheight("CULOARE1")/2-15)&&y<(y_buton_culoare1+textheight("CULOARE1")/2 +15)){ ///pozitionarea butonului pentru culoare
                if(culoare1==14) culoare1=0; //daca am depasit paleta de culori, revin la inceput; 15=alb imposibil prin modul in care deseneaza libraria
                else if(culoare1+1==culoare2){
                    culoare1+=2;
                    if(culoare1>=14) culoare1=0;
                }
                else culoare1++;
 
                setari.culoare1=culoare1;
                buton(x_buton_culoare1,y_buton_culoare1,"CULOARE1",culoare1);
            }
 
             if(x>(x_buton_culoare2-textwidth("CULOARE1")/2-15)&&x<(x_buton_culoare2+textwidth("CULOARE1")/2+15) && y>(y_buton_culoare2-textheight("CULOARE1")/2-15)&&y<(y_buton_culoare2+textheight("CULOARE1")/2 +15)){ ///pozitionarea butonului pentru culoare
                if(culoare2==14) culoare2=0; //daca am depasit paleta de culori, revin la inceput; 15=alb imposibil prin modul in care deseneaza libraria
                else if(culoare1==culoare2+1){
                    culoare2+=2;
                    if(culoare2>=14) culoare2=0;
                }
                else culoare2++;
 
                setari.culoare2=culoare2;
                buton(x_buton_culoare2,y_buton_culoare2,"CULOARE2",culoare2);
            }
 
            if(x>(x_buton_metodainput-textwidth("CONSOLA1")/2-15) && x<(x_buton_metodainput+textwidth("CONSOLA1")/2+15) && y> (y_buton_metodainput-textheight("C")/2-15) &&y<(y_buton_metodainput+textheight("C")/2+15)){
                if(setari.metodaInput==1) setari.metodaInput=0;///adica consola
                else setari.metodaInput=1;///default, introducere in mod grafic
 
                if(setari.metodaInput==1)
                    buton(x_buton_metodainput, y_buton_metodainput, "MOD GRAFIC", LIGHTRED);
                else buton(x_buton_metodainput, y_buton_metodainput, ">[CONSOLA]", BLACK);
            }
            if(x>(x_buton_afisareDirecta-textwidth("AFISARE DIRECTA: DA")/2-15) && x<(x_buton_afisareDirecta+textwidth("AFISARE DIRECTA: DA")/2+15) && y> (y_buton_afisareDirecta-textheight("C")/2-15) &&y<(y_buton_afisareDirecta+textheight("C")/2+15)){
                if(setari.afisareDirecta==true) setari.afisareDirecta=false;///nu
                else setari.afisareDirecta=true;///default, da
 
                if(setari.afisareDirecta==true)
                    buton(x_buton_afisareDirecta, y_buton_afisareDirecta, "AFISARE DIRECTA: DA", LIGHTGREEN);
                else buton(x_buton_afisareDirecta, y_buton_afisareDirecta, "AFISARE DIRECTA: NU", BROWN);
            }
            if(x>(x_buton_asteapta5sec-textwidth("Asteapta 5 sec: DA")/2-15) && x<(x_buton_asteapta5sec+textwidth("Asteapta 5 sec: DA")/2+15) && y> (y_buton_asteapta5sec-textheight("C")/2-15) &&y<(y_buton_asteapta5sec+textheight("C")/2+15)){
                if(setari.asteapta5secunde==true) setari.asteapta5secunde=false;///nu
                else setari.asteapta5secunde=true;///default, da
 
                if(setari.asteapta5secunde==true)
                    buton(x_buton_asteapta5sec, y_buton_asteapta5sec, "Asteapta 5 sec: DA", LIGHTCYAN);
                else buton(x_buton_asteapta5sec, y_buton_asteapta5sec, "Asteapta 5 sec: NU", BLUE);
            }
            if(x>(x_buton_salvareInIstoric-textwidth("Asteapta 5 sec: DA")/2-15) && x<(x_buton_salvareInIstoric+textwidth("Asteapta 5 sec: DA")/2+15) && y> (y_buton_salvareInIstoric-textheight("C")/2-15) &&y<(y_buton_salvareInIstoric+textheight("C")/2+15)){
                if(setari.salvareInIstoric==true) setari.salvareInIstoric=false;///nu
                else setari.salvareInIstoric=true;///default, da
 
                if(setari.salvareInIstoric==true)
                    buton(x_buton_salvareInIstoric, y_buton_salvareInIstoric, "ISTORIC FORMULE: DA", LIGHTGRAY);
                else buton(x_buton_salvareInIstoric, y_buton_salvareInIstoric, "ISTORIC FORMULE: NU", BLACK);
            }
            if(x>(x_buton_salvareImagine-textwidth("Salvare formula in imagine: DA")/2-15) && x<(x_buton_salvareImagine+textwidth("Salvare formula in imagine: NU")/2+15) && y> (y_buton_salvareImagine-textheight("C")/2-15) &&y<(y_buton_salvareImagine+textheight("C")/2+15)){
                if(setari.salvareFormulaInPoza==true) setari.salvareFormulaInPoza=false;///nu
                else setari.salvareFormulaInPoza=true;///default, da
 
                if(setari.salvareFormulaInPoza==true)
                    buton(x_buton_salvareImagine, y_buton_salvareImagine, "Salvare formula in imagine: DA", GREEN);
                else buton(x_buton_salvareImagine, y_buton_salvareImagine, "Salvare formula in imagine: NU", RED);
            }
 
 
            if(x>(x_buton_albnegru-textwidth("W/B")/2-15) && x<(x_buton_albnegru+textwidth("B/W")/2+15) && y> (y_buton_albnegru-textheight("C")/2-15) &&y<(y_buton_albnegru+textheight("C")/2+15)){
                if(setari.culoare1==WHITE && setari.culoare2==BLACK){
                setari.culoare1=BLACK;
                setari.culoare2=WHITE;
                buton(x_buton_albnegru, y_buton_albnegru, "B/W", BLACK);
                }
                else{
                    setari.culoare1=WHITE;
                    setari.culoare2=BLACK;
                buton(x_buton_albnegru, y_buton_albnegru, "W/B", BLACK);
                }
 
 
            }
            if(x>x_buton_exit-20 && x<x_buton_exit+20 && y> y_buton_exit-15 &&y<y_buton_exit+15){
                proceduraSalvareSetari();
                exit=true;
                //goto close;
            }
        }
    }
 
 
//getch();
//close:
closegraph();
}
 
/** PRELUCRAREA SIRULUI INFIXAT **/
 
void creareSirTokenizat() {
    int i, j;
    for (i = 0; i < formula_infixata.length(); i++) {
        string sir;
        if (isdigit(formula_infixata[i])) {
            j = i;
            while (isdigit(formula_infixata[j]) || (formula_infixata[j] == '.')) {
                sir.append(1, formula_infixata[j]);
                j++;
            }
            sirDeTokenuri[nrMembri++].append(sir);
            i = j - 1;
 
        }
        else if (isalpha(formula_infixata[i])) {
            j = i;
            while (isalpha(formula_infixata[j]) || formula_infixata[j]=='_' || isdigit(formula_infixata[j]) || formula_infixata[j]=='.' || formula_infixata[j]=='=') {
                sir.append(1, formula_infixata[j]);
                j++;
            }
            sirDeTokenuri[nrMembri].append(sir);
            nrMembri++;
            i = j - 1;
 
        }
        else {
            sir.append(1, formula_infixata[i]);
            sirDeTokenuri[nrMembri].append(sir);
            nrMembri++;
        }
 
    }
    for (int i = 0; i <= nrMembri; i++)
        cout << sirDeTokenuri[i] << " ";
}
 
bool esteOperator(string c) {
    if (c == "+" || c == "*" || c == "/" || c == "^" || c == "-")
        return true;
    return false;
}
bool esteOperatorUnar(string c) {
    if (c == "sin" || c == "cos" || c == "sqrt" || c == "ln" || c == "lg" || c == "abs" || c=="!" || c=="tg" || c=="ctg" || c=="arcsin" || c == "arccos" || c == "arctg" || c == "arcctg")
        return true;
    return false;
}
bool esteAlfa(string c) {
    if (esteOperatorUnar(c)) return false;
    else return (strspn(c.c_str(), "_abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789.") == c.length());
}
bool esteNumar(string c) {
    return (strspn(c.c_str(), ".0123456789") == c.length());
}
bool esteOperand(string c) {
    if (c.empty())
        return false;
    if (esteAlfa(c))///nu stiu cat de stabil va fi!!!
        return true;
    if (esteNumar(c))
        return true;
    return false;
}
int precedenta(string op) {
    if (op == "+" || op == "-")
        return 1;
    if (op == "*" || op == "/")
        return 2;
    if (op == "^")
        return 3;
    if (op == "sin" || op == "cos" || op == "sqrt" || op == "ln" || op == "abs" || op == "lg"||op=="!" || op=="tg" || op=="ctg" || op=="arcsin" ||op == "arccos" || op == "arctg" || op == "arcctg")
        return 4;
    return 0;
}
bool precedentaMaiMareSauEgala(string op1, string op2) {
    int p1 = precedenta(op1);
    int p2 = precedenta(op2);
    if (p1 == p2) {
        if (op1 == "^")
            return false;
        return true;
    }
    return  (p1 > p2 ? true : false);
}
bool esteParanteza(string c);
 
void TokenizatLaPostfixat() {
 
    stack<string> S;
    string ch;
    S.push("(");
    sirDeTokenuri[nrMembri].append(")");
 
    int k = 0;
    for (int i = 0; i <= nrMembri; i++) {
        ch=sirDeTokenuri[i];///assign
 
        if (ch == "(") S.push(ch);
        else if (esteOperatorUnar(ch)) S.push(ch);
        else if (ch == "[") S.push(ch);
        else if (esteOperator(ch)) {
            while (!S.empty() && precedentaMaiMareSauEgala(S.top(), ch)) {
                sirPostfixat[k++] = S.top();
                S.pop();
            }
            S.push(ch);
        }
        else if (esteOperand(ch)) sirPostfixat[k++] = ch;
        else if (ch == ")") {
            while (!S.empty() && S.top() != "(") {
                if (esteParanteza(S.top())) {
                    cout << "Eroare la parsare! Motiv: caracterul "<<S.top()<<" a fost tratat ca operand, semn ca lipsesc paranteze.";
                    exit(1);
                }
                sirPostfixat[k++] = S.top();
                S.pop();
            }
            S.pop();
        }
        else if (ch == "]") {
            while (!S.empty() && S.top() != "[") {
                sirPostfixat[k++] = S.top();
                S.pop();
            }
            S.pop();
        }
    }
 
    if (!S.empty()) cout << "Eroare la parsare! Motiv: Au ramas elemente in stiva, semn ca lipsesc paranteze";
 
}
 
 
/** SANITIZAREA STRINGULUI INFIXAT **/ //+ detectia unei sintaxe gresitede
 
bool esteParanteza(string c) {
    if (c == ")" || c == "(" || c == "]" || c == "[" || c == "{" || c == "}") return true;
    return false;
}
bool esteParantezaDeschisa(string c) {
    if (c == "(" || c == "[" || c == "{") return true;
    return false;
}
bool esteParantezaInchisa(string c) {
    if (c == ")" || c == "]" || c == "}") return true;
    return false;
}
bool parantezePereche(string a, string b) {
    if ((a == "(" && b == ")") || (a == "[" && b == "]") || (a == "{" && b == "}")) return true;
    return false;
}
 
int contor_vector_indici=0;
void sanitizare() {
 
    for (int i = 0; i < nrMembri; i++) {
        if(esteNumar(sirDeTokenuri[i])&&esteOperand(sirDeTokenuri[i+1])){
            for (int j = nrMembri; j > i; j--)
                sirDeTokenuri[j] = sirDeTokenuri[j - 1];
            sirDeTokenuri[i + 1] = "*";
            nrMembri++;
        }
    }
 
    for (int i = 0; i < nrMembri; i++) { //cazurile:  (/[/{-a=> (/[/{ (0-a) | )/]/}-fctUnara[] => )/]/}-fctUnara[]
        if ((i == 0 && sirDeTokenuri[i] == "-") ||
                (sirDeTokenuri[i] == "-" && esteParantezaDeschisa(sirDeTokenuri[i - 1]) && !esteOperand(sirDeTokenuri[i - 1]))
                ) /// cazul 1-(-(1-2))+1
        {
            if (esteOperand(sirDeTokenuri[i+1]) && (i==0 || !esteOperand(sirDeTokenuri[i - 1]))) {
                if(sirDeTokenuri[i+2]=="^" || sirDeTokenuri[i+2]=="*"){//||sirDeTokenuri[i+2]=="/"
                    for (int j = nrMembri + 1; j > i; j--)
                        sirDeTokenuri[j] = sirDeTokenuri[j - 1];
                    sirDeTokenuri[i] = "0";
                    nrMembri++;
                }
                else
                {
                for (int j = nrMembri + 3; j> i + 3; j--)
                    sirDeTokenuri[j] = sirDeTokenuri[j - 3];
                sirDeTokenuri[i + 4 ] = ")";
                sirDeTokenuri[i + 3] = sirDeTokenuri[i + 1];
                sirDeTokenuri[i + 2] = sirDeTokenuri[i];
                sirDeTokenuri[i + 1] = "0";
                sirDeTokenuri[i] = "(";
                nrMembri += 3;
                }
            }
            else if (esteOperatorUnar(sirDeTokenuri[i + 1]) ) {
                stack<string> stiva;
                int nrTokenuriInFunctie = 0; //cu tot cu parantezele patrate
 
                for (int j = i + 2; j < nrMembri; j++)
                    if (esteParantezaDeschisa(sirDeTokenuri[j])) {
                        stiva.push(sirDeTokenuri[j]); // a fost inchisa o paranteza
                        nrTokenuriInFunctie++;
                    }
                    else if (esteParantezaInchisa(sirDeTokenuri[j])) {
                        if (stiva.empty()) {
                            printf("Eroare! Am detectat la pozitia %d un operator unar care nu are o paranteza de inceput", j);
                            return; //trebuie un proper exit
                        }
 
                        if (parantezePereche(stiva.top(), sirDeTokenuri[j])) stiva.pop();
                        nrTokenuriInFunctie++;
 
                        if (stiva.empty()) break;
                    }
                    else nrTokenuriInFunctie++;
 
                for (int j = nrMembri + 3; j > i + nrTokenuriInFunctie + 3; j--)
                    sirDeTokenuri[j] = sirDeTokenuri[j - 3];
                sirDeTokenuri[i + nrTokenuriInFunctie + 4] = ")";
                for (int j = i + nrTokenuriInFunctie + 2; j > i; j--)
                    sirDeTokenuri[j+1] = sirDeTokenuri[j - 1];
                sirDeTokenuri[i + 1] = "0";
                sirDeTokenuri[i] = "(";
                nrMembri += 3;
            }
        }
    }
 
    for (int i = 1; i < nrMembri; i++) {
        if (sirDeTokenuri[i] == "+" && esteParantezaDeschisa(sirDeTokenuri[i - 1])) {
            for (int j = i; j < nrMembri; j++) sirDeTokenuri[j] = sirDeTokenuri[j + 1];
                nrMembri -= 1;
        }
    }
 
    for (int i = 0; i < nrMembri; i++) {
        if (sirDeTokenuri[i] == ")" || sirDeTokenuri[i] == "(" || sirDeTokenuri[i] == "]" || sirDeTokenuri[i] == "["|| sirDeTokenuri[i] == "}" || sirDeTokenuri[i] == "{");
        else nrTokenuri++;
    }
 
 
 
    ///sanitizez suma, produs, integrale
    ///exemplu: suma_n=2_inf{k}         suma_n=0_3{(x#-4)}+5
 
 
    for(int i=0;i<nrMembri;i++){ ///se precizeaza care este operatorul dominant, daca exista, cu #
        if(sirDeTokenuri[i].find("suma_")!= string::npos ||
           sirDeTokenuri[i].find("produs_")!= string::npos ||
           sirDeTokenuri[i].find("IN")!= string::npos && sirDeTokenuri[i].find("IN")==0 ||
           sirDeTokenuri[i].find("ID_")!= string::npos) //nu credeam ca putea veni ziua cand ma voi bucura ca am deja o functie oarba deja scrisa
        {
            int indice_vector_indici=0;
            int tip;
            if(sirDeTokenuri[i].find("suma_")!=string::npos) tip=1;
            if(sirDeTokenuri[i].find("produs_")!=string::npos) tip=2;
            if(sirDeTokenuri[i].find("IN")!=string::npos) tip=3;
            if(sirDeTokenuri[i].find("ID_")!=string::npos) tip=4;
 
            string jos="", sus="";
            if(tip==1){
                    int check=0;
                    for(int j=5;j<sirDeTokenuri[i].size();j++){
                        if(check==1) sus.push_back(sirDeTokenuri[i][j]);
                        if(sirDeTokenuri[i][j]!='_' && check==0)jos.push_back(sirDeTokenuri[i][j]);
                        else if(check==0) check=1;
                    }
            }
            if(tip==2){
                    int check=0;
                    for(int j=7;j<sirDeTokenuri[i].size();j++){
                        if(check==1) sus.push_back(sirDeTokenuri[i][j]);
                        if(sirDeTokenuri[i][j]!='_' && check==0)jos.push_back(sirDeTokenuri[i][j]);
                        else if(check==0)check=1;
                    }
            }///pentru integrala nedefinita nu mai am ce verifica
            if(tip==4){
                    int check=0;
                    for(int j=3;j<sirDeTokenuri[i].size();j++){
                        if(check==1) sus.push_back(sirDeTokenuri[i][j]);
                        if(sirDeTokenuri[i][j]!='_' && check==0)jos.push_back(sirDeTokenuri[i][j]);
                        else if(check==0)check=1;
                    }
            }
 
            ///automat sirdetokenuri[i+1] este {
            int pozitie_operatie_dominanta;
            bool contine_doar_operand=false;
            if(sirDeTokenuri[i+3]=="}") {
                    contine_doar_operand=true; /// continutul e operand
                for(int rearanjare=i+3;rearanjare<nrMembri;rearanjare++)
                    sirDeTokenuri[rearanjare]=sirDeTokenuri[rearanjare+1];
                    //if(tip==3 || tip==4)
                        indice_vector_indici=-2; ///ceva e dubios aici, dar pare ca rezolva
                    //else indice_vector_indici=-1;
                for(int q=0; q < i+3; q++)
                    if(esteOperand(sirDeTokenuri[q]))
                            indice_vector_indici++;
            }
            else
                for(int p=i+3;sirDeTokenuri[p]!="}";p++){ ///se pune pe operator
                    if(sirDeTokenuri[p]=="#"){
                        pozitie_operatie_dominanta=p-2; ///pentru ca elimin si { }
 
                        for(int q=0; q < p ; q++)
                            if(esteOperator(sirDeTokenuri[q]) || esteOperatorUnar(sirDeTokenuri[q]))
                                indice_vector_indici++; ///numar cati operatori am inainte de operatorul dominant
 
                        for(int rearanjare=p; rearanjare < nrMembri; rearanjare++)///elimin #-ul
                            sirDeTokenuri[rearanjare]=sirDeTokenuri[rearanjare+1];
                        nrMembri--;
 
                        int pozitie_acolada_terminala = p;
                        for(; sirDeTokenuri[pozitie_acolada_terminala]!="}"; pozitie_acolada_terminala++); ///am gasit unde este }
                        for(int rearanjare2 = pozitie_acolada_terminala; rearanjare2<nrMembri; rearanjare2++)
                            sirDeTokenuri[rearanjare2]=sirDeTokenuri[rearanjare2+1];
                        nrMembri--;
                        break;
                    }
                    else if(sirDeTokenuri[p+1]=="}")
                        {cout<<"Eroare la gasirea operatiei dominante! Nu am gasit specificatorul #!"<<endl;exit(1);}
 
                }
 
                for(int l=i;l<nrMembri;l++)
                    sirDeTokenuri[l]=sirDeTokenuri[l+2]; ///elimin tokenii suma_..._... si {
 
                if(contine_doar_operand)nrMembri-=2;
                else nrMembri-=2;///skepsis major
 
                arr_indici_noduri_speciale[contor_vector_indici].jos=jos;
                arr_indici_noduri_speciale[contor_vector_indici].sus=sus;
                arr_indici_noduri_speciale[contor_vector_indici].tip=tip;
                if(contine_doar_operand)arr_indici_noduri_speciale[contor_vector_indici].stocat_pe_operand=true;
                arr_indici_noduri_speciale[contor_vector_indici].indice_pozitie=indice_vector_indici;
                contor_vector_indici++;
 
            if(contine_doar_operand)nrTokenuri-=1;
            else nrTokenuri-=2;///improv?!
        }
    }
 
    cout<<endl<<nrMembri<<" "<<nrTokenuri<<endl;
 
}
 
   int contor_operanzi=0;
   int contor_operatori=0;
   int alt_contor=0;
 
   bool check=false;
void atribuireNoduriSpeciale(arbore*& a,int referinta_inapoi_la_vector, bool tip_cautat, int indice){ ///tip_cautat e dupa operand sau operator
 
    if (a!=NULL) {
        atribuireNoduriSpeciale(a->stanga, referinta_inapoi_la_vector, tip_cautat, indice);
        if(tip_cautat==true){
            if(esteOperand(a->info))
                if(check==false)
                    if(contor_operanzi<indice ) contor_operanzi++;//&& !check
                    else {
                        check=true;
                        a->indice_nod_special=referinta_inapoi_la_vector;
                        return;
                    }
        }
        else{
            if(esteOperator(a->info) || esteOperatorUnar(a->info))
                if(check==false)
                    if(contor_operatori<indice) contor_operatori++;
                    else {
                        check=true;
                        a->indice_nod_special=referinta_inapoi_la_vector;
                        return;
                    }
        }
 
        atribuireNoduriSpeciale(a->unar, referinta_inapoi_la_vector, tip_cautat, indice);
        atribuireNoduriSpeciale(a->dreapta, referinta_inapoi_la_vector, tip_cautat, indice);
    }
 
 
}
 
void atribNoduriSpec(){
    for(int i=0;i<contor_vector_indici;i++){
        atribuireNoduriSpeciale(ArbConstruit, i, arr_indici_noduri_speciale[i].stocat_pe_operand, arr_indici_noduri_speciale[i].indice_pozitie);
        contor_operanzi=0;
        contor_operatori=0;
        check=false;
    }
}
 
void inordine(arbore* Arb) {
    if (Arb) {
        inordine(Arb->stanga);
        cout << Arb->info;
        inordine(Arb->unar);
        inordine(Arb->dreapta);
    }
}
 
arbore* nodNou(string expresie) {
    arbore* aux = new arbore;
    aux->stanga = aux->dreapta = aux->unar = NULL;
    aux->info = expresie;
    return aux;
}
arbore* construiesteArbore() {
    stack<arbore*> stiva;
    arbore* t, * t1, * t2, * t3;
 
    for (int i = 0; i < nrTokenuri; i++) {
        if (sirPostfixat[i] != "-" && esteOperand(sirPostfixat[i])) {
            t = nodNou(sirPostfixat[i]);
            stiva.push(t);
        }
        else if (esteOperatorUnar(sirPostfixat[i])) {
            t = nodNou(sirPostfixat[i]);
            t3 = stiva.top();
            t->unar = t3;
            stiva.pop();
            stiva.push(t);
        }
        else {
            t = nodNou(sirPostfixat[i]);
 
            t1 = stiva.top();
            stiva.pop();
            t2 = stiva.top();
            stiva.pop();
 
            t->dreapta = t1;
            t->stanga = t2;
 
            stiva.push(t);
        }
    }
 
    t = stiva.top();
    stiva.pop();
 
    return t;
}
 
 
 
 
/** PARTEA GRAFICA - DESENAREA FORMULEI **/
 
bool primulSemnEMinus(arbore* a){ // prototip, rezolva cazurile k-(-k+2), -3-(-4)+(-333+4-4-3)
    if(a!=NULL) {
        if(a->info=="-" && a->stanga->info=="0") return true;
        else primulSemnEMinus(a->stanga);
    }
    else return false;
}
 
bool cazunic = false;
int lungimeComponentaC(arbore* a){
 
    if(a->info=="/") return 1+max(lungimeComponentaC(a->stanga),lungimeComponentaC(a->dreapta)); ///+1
    else if(esteOperand(a->info)){
        if(a->info.find("log_") != string::npos ) return a->info.size()-2;
        else if(a->info.find("_") != string::npos) return a->info.size()-1;
        else if(a->info.find("pi")==0) return 1;
        if(a->info=="0") return 0;
        if(a->info.size()%2 == 0) return a->info.size()+1; ///idee proasta, nu stiu in ce masura va afecta toata situatia
        else return a->info.size();//reliable?
    }
    else if(a->info == "*"){
        if(a->stanga->info=="+" || a->stanga->info=="-" || a->dreapta->info=="-" || a->dreapta->info=="+")
            return 3 + lungimeComponentaC(a->stanga)+lungimeComponentaC(a->dreapta)+1;///are paranteze ///aici n-ar fi un +1 ///este 2 reliable?!!
        //else if(esteNumar(a->stanga->info) && esteOperand(a->dreapta->info)) return lungimeComponentaC(a->stanga)+lungimeComponentaC(a->dreapta);
        else  return lungimeComponentaC(a->stanga)+lungimeComponentaC(a->dreapta)+1;
    }
    else if(a->info== "+" || a->info == "-"){
            if(a->info=="-" && (a->dreapta->info=="-" || a->dreapta->info=="+"))//( && primulSemnEMinus(a->dreapta) )||
                return lungimeComponentaC(a->stanga)+lungimeComponentaC(a->dreapta)+3;
            if(a->stanga->info=="0" && a->info=="-" && minusflag==false && cazunic==true){
                cazunic=false; return lungimeComponentaC(a->stanga)+lungimeComponentaC(a->dreapta)+3; } ///cazurile cu paranteze 1-(-1)
            else return lungimeComponentaC(a->stanga)+lungimeComponentaC(a->dreapta)+1;
    }
    else if(a->info=="^") {
        if(esteOperator(a->stanga->info)) return lungimeComponentaC(a->stanga)+(90*lungimeComponentaC(a->dreapta))/100+3;///+3
        else return lungimeComponentaC(a->stanga)+(90*lungimeComponentaC(a->dreapta))/100+1;///+1
    }
    else if(a->info=="abs" || a->info=="sqrt") return lungimeComponentaC(a->unar)+1;
    else if(a->info=="!"){
        if(esteOperator(a->unar->info))return 3 + lungimeComponentaC(a->unar);
        else return 1 + lungimeComponentaC(a->unar);
    }
    else if(esteOperatorUnar(a->info)) return lungimeComponentaC(a->unar) + a->info.size()+2;//+2 pentru paranteze ///a->info!="abs" && a->info!="sqrt" && a->info!="!" &&
 
return 0;
}
int lungimeComponenta(arbore* a){
    if(a){
        if(a->indice_nod_special != -1) return 3 + lungimeComponentaC(a);
        else return lungimeComponentaC(a);
 
    }
}
 
 
int latimeComponenta(arbore* a){
    if(a==NULL) return 0;
    else if(esteOperatorUnar(a->info)&&a->info!="sqrt") return latimeComponenta(a->unar)+1;
    else if(a->info=="/") return latimeComponenta(a->stanga)+latimeComponenta(a->dreapta)+1;//+2
    else if(a->info=="^") return latimeComponenta(a->stanga)+ 1*latimeComponenta(a->dreapta)/2;
    else if(a->info=="sqrt") return latimeComponenta(a->stanga) + latimeComponenta(a->unar) + latimeComponenta(a->dreapta);//
    else if(esteOperator(a->info) && a->info!="/" && a->info!="^") return max(latimeComponenta(a->stanga), latimeComponenta(a->dreapta));
    else if(esteOperand(a->info)) return 1;
    return 0;
}
int latimeFractieSus(arbore* a){ ///probleme serioase cu fractiile si puterile
    if(a==NULL)return 0;
    else if(esteOperand(a->info)) return 1;
    else if(esteOperatorUnar(a->info)) return latimeFractieSus(a->unar)+1;//
    else if(a->info=="/") {
        //if(esteOperator(a->stanga->info)) return latimeFractieSus(a->stanga)+1;
        return latimeFractieSus(a->dreapta)+2;//+1
    }
    else if(a->info=="^" || a->info=="sqrt") return latimeFractieSus(a->stanga) + latimeFractieSus(a->unar);//+ latimeFractieSus(a->dreapta) ///daca apar erori de afisare la asta, de adaugat un +1
    else if(esteOperator(a->info) && a->info!="/" && a->info!="^") return max(latimeFractieSus(a->stanga), latimeFractieSus(a->dreapta)); ///aici nu era un 1+, vedem ce face
 
    return 0;
}
int latimeFractieJos(arbore* a){
    if(a==NULL) return 0;
    else if(esteOperand(a->info)) return 1;
    else if(esteOperatorUnar(a->info)) return latimeFractieJos(a->unar)+1;//
    else if(a->info=="/"){
        //if(esteOperator(a->dreapta->info)) return latimeFractieJos(a->dreapta)+1;
        return latimeFractieJos(a->stanga)+2;//+1
    }
    else if(a->info=="^" || a->info=="sqrt") return 1+latimeFractieJos(a->stanga) + latimeFractieJos(a->unar);//+ latimeFractieJos(a->dreapta)
    else if(esteOperator(a->info) && a->info!="/" && a->info!="^") return max(latimeFractieJos(a->stanga), latimeFractieJos(a->dreapta));//
    return 0;
}
 
 
int compensareOB(arbore* a){
    if(a!=NULL){
        if(esteOperand(a->info)) return 0;
        if(esteOperatorUnar(a->info)) return lungimeComponenta(a->unar);
        if(lungimeComponenta(a->stanga)>lungimeComponenta(a->dreapta))
            return (lungimeComponenta(a->stanga) - lungimeComponenta(a->dreapta))*textwidth("x");
        else return (lungimeComponenta(a->dreapta) - lungimeComponenta(a->stanga)+1)*textwidth("x");
    }
    else return 0;
}
int compensareOBt(arbore* a){
    if(a!=NULL){
        if(esteOperand(a->info)) return 0;
        if(esteOperatorUnar(a->info)) return lungimeComponenta(a->unar);
        if(lungimeComponenta(a->stanga)>lungimeComponenta(a->dreapta))
            return (lungimeComponenta(a->stanga) - lungimeComponenta(a->dreapta))*textwidth("x");
        else return (lungimeComponenta(a->stanga) - lungimeComponenta(a->dreapta))*textwidth("x");
    }
    else return 0;
}
 
void sigma(int xcomp, int y, int lungime){
    settextstyle(0,HORIZ_DIR,4);
    int x=xcomp;//-lungime/2+textwidth("(")/2
    line(x-lungime-textwidth("(")*3,y-textheight("x"), x-lungime-textwidth("("), y-textheight("x"));
    line(x-lungime-textwidth("(")*2, y,x-lungime-textwidth("(")*3,y-textheight("x"));
    line(x-lungime-textwidth("(")*2, y,x-lungime-textwidth("(")*3,y+textheight("x"));
    line(x-lungime-textwidth("(")*3,y+textheight("x"), x-lungime-textwidth("("), y+textheight("x"));
    settextstyle(0,HORIZ_DIR,coeficientScala);
}
void produs(int x, int y, int lungime){
    settextstyle(0,HORIZ_DIR,4);
    line(x-lungime-textwidth("(")*3,y-textheight("x"), x-lungime-textwidth("("), y-textheight("x"));
    line(x-lungime-textwidth("(")*5/3,y+textheight("x"),x-lungime-textwidth("(")*5/3,y-textheight("x"));
    line(x-lungime-textwidth("(")*7/3,y+textheight("x"),x-lungime-textwidth("(")*7/3,y-textheight("x"));
    settextstyle(0,HORIZ_DIR,coeficientScala);
}
void radical(int x,int y, int lungime, int latime){
line(x-lungime/2-10,y,x-lungime/2-5,y+latime/2+10);
line(x-lungime/2-5,y+latime/2+10, x-lungime/2,y-latime/2-15);
line(x-lungime/2,y-latime/2-15,x+lungime/2,y-latime/2-15);
}
void radicalC(int x,int y,int lungimeComponenta, int latimeComponenta){
radical(x,y,lungimeComponenta*textwidth("X"),latimeComponenta*textheight("X"));
}
void integrala(int x, int y, int lungimeComponenta, int latimeComponenta){
line(x-lungimeComponenta/2+2,y+latimeComponenta/2, x-lungimeComponenta/2+2,y-latimeComponenta/2); //x-lungimeComponenta/2+lungime cursor sus/jos!
line(x-lungimeComponenta/2+2,y-latimeComponenta/2, x-lungimeComponenta/2+12,y-latimeComponenta/2);
line(x-lungimeComponenta/2+2,y+latimeComponenta/2, x-lungimeComponenta/2-8,y+latimeComponenta/2);
cout<<"desenat"<<endl;
}
void pi(int x, int y, int latimeFont){
    line(x-textwidth("x")/2, y-latimeFont/3,x+textwidth("x")/2, y-latimeFont/3);//
    line(x-textwidth("x")/4, y-latimeFont/3,x-textwidth("x")/4, y+latimeFont/3);
    line(x+textwidth("x")/4, y-latimeFont/3,x+textwidth("x")/4, y+latimeFont/3);
}
 
 
void draw(arbore* a, Cursor c, int marimeFont){//&
    if(a->indice_nod_special!=-1){
        if(arr_indici_noduri_speciale[a->indice_nod_special].tip==1){
            if(arr_indici_noduri_speciale[a->indice_nod_special].stocat_pe_operand==true){
                settextstyle(0,HORIZ_DIR,3);
                const char* str1=arr_indici_noduri_speciale[a->indice_nod_special].sus.c_str();
                outtextxy(c.x-lungimeComponentaC(a)-textwidth("(")*5+a->info.size()*textwidth("x"),c.y-textheight("x")*2-+textheight("x")/2, (char*)str1);//
                const char* str2=arr_indici_noduri_speciale[a->indice_nod_special].jos.c_str();
                outtextxy(c.x-lungimeComponentaC(a)-textwidth("(")*5+a->info.size()*textwidth("x"),c.y+textheight("x")+textheight("x")/2, (char*)str2);
                settextstyle(0,HORIZ_DIR,marimeFont);
                sigma(c.x+a->info.size()*textwidth("x"),c.y,lungimeComponentaC(a)*textwidth("x"));
            }
            else{
                settextstyle(0,HORIZ_DIR,3);
                const char* str1=arr_indici_noduri_speciale[a->indice_nod_special].sus.c_str();
                outtextxy(c.x-lungimeComponentaC(a)-textwidth("(")*5,c.y-textheight("x")*2-+textheight("x")/2, (char*)str1);//
                const char* str2=arr_indici_noduri_speciale[a->indice_nod_special].jos.c_str();
                outtextxy(c.x-lungimeComponentaC(a)-textwidth("(")*5,c.y+textheight("x")+textheight("x")/2, (char*)str2);
                settextstyle(0,HORIZ_DIR,marimeFont);
                sigma(c.x+lungimeComponenta(a)*textwidth("x")/3,c.y,lungimeComponentaC(a)*textwidth("x"));
            }
        }
        if(arr_indici_noduri_speciale[a->indice_nod_special].tip==2){
            if(arr_indici_noduri_speciale[a->indice_nod_special].stocat_pe_operand==true){
                settextstyle(0,HORIZ_DIR,3);
                const char* str1=arr_indici_noduri_speciale[a->indice_nod_special].sus.c_str();
                outtextxy(c.x-lungimeComponentaC(a)-textwidth("(")*5+a->info.size()*textwidth("x"),c.y-textheight("x")*2-+textheight("x")/2, (char*)str1);//
                const char* str2=arr_indici_noduri_speciale[a->indice_nod_special].jos.c_str();
                outtextxy(c.x-lungimeComponentaC(a)-textwidth("(")*5+a->info.size()*textwidth("x"),c.y+textheight("x")+textheight("x")/2, (char*)str2);
                settextstyle(0,HORIZ_DIR,marimeFont);
                produs(c.x+a->info.size()*textwidth("x"),c.y,lungimeComponentaC(a)*textwidth("x"));
            }
            else{
                settextstyle(0,HORIZ_DIR,3);
                const char* str1=arr_indici_noduri_speciale[a->indice_nod_special].sus.c_str();
                outtextxy(c.x-lungimeComponentaC(a)-textwidth("(")*5,c.y-textheight("x")*2-+textheight("x")/2, (char*)str1);//
                const char* str2=arr_indici_noduri_speciale[a->indice_nod_special].jos.c_str();
                outtextxy(c.x-lungimeComponentaC(a)-textwidth("(")*5,c.y+textheight("x")+textheight("x")/2, (char*)str2);
                settextstyle(0,HORIZ_DIR,marimeFont);
                produs(c.x+lungimeComponenta(a)*textwidth("x")/3,c.y,lungimeComponentaC(a)*textwidth("x"));
            }
        }
        if(arr_indici_noduri_speciale[a->indice_nod_special].tip==3){
            if(arr_indici_noduri_speciale[a->indice_nod_special].stocat_pe_operand==true)
                integrala(c.x+textwidth("x")/2,c.y,lungimeComponenta(a)*textwidth("x"),textheight("x")*2);
            else
                integrala(c.x+textwidth("x")/2,c.y,lungimeComponenta(a)*textwidth("x"),textheight("x")*2);
            settextstyle(0,HORIZ_DIR,3);
            string s1="dx";
            const char* str2=s1.c_str();
            outtextxy(c.x+lungimeComponentaC(a)*textwidth("x"),c.y-textheight("x")/2, (char*)str2);
            settextstyle(0,HORIZ_DIR,marimeFont);
        }
        if(arr_indici_noduri_speciale[a->indice_nod_special].tip==4){
            if(arr_indici_noduri_speciale[a->indice_nod_special].stocat_pe_operand==true){
                settextstyle(0,HORIZ_DIR,3);
                const char* str1=arr_indici_noduri_speciale[a->indice_nod_special].sus.c_str();
                outtextxy(c.x-lungimeComponentaC(a)-textwidth("(")*3+a->info.size()*textwidth("x"),c.y-textheight("x")*2-+textheight("x"), (char*)str1);//
                const char* str2=arr_indici_noduri_speciale[a->indice_nod_special].jos.c_str();
                outtextxy(c.x-lungimeComponentaC(a)-textwidth("(")*3+a->info.size()*textwidth("x"),c.y+textheight("x")+textheight("x"), (char*)str2);
                settextstyle(0,HORIZ_DIR,marimeFont);
                integrala(c.x+textwidth("x")/2,c.y,lungimeComponenta(a)*textwidth("x"),textheight("x")*2);
            }
            else{
                settextstyle(0,HORIZ_DIR,3);
                const char* str1=arr_indici_noduri_speciale[a->indice_nod_special].sus.c_str();
                outtextxy(c.x-lungimeComponentaC(a)-textwidth("(")*4,c.y-textheight("x")*2-+textheight("x"), (char*)str1);//
                const char* str2=arr_indici_noduri_speciale[a->indice_nod_special].jos.c_str();
                outtextxy(c.x-lungimeComponentaC(a)-textwidth("(")*4,c.y+textheight("x")+textheight("x"), (char*)str2);
                settextstyle(0,HORIZ_DIR,marimeFont);
 
                integrala(c.x+textwidth("x")/2,c.y,lungimeComponenta(a)*textwidth("x"),textheight("x")*2);
            }
            settextstyle(0,HORIZ_DIR,3);
            string s1="dx";
            const char* str2=s1.c_str();
            outtextxy(c.x+lungimeComponentaC(a)*textwidth("x"),c.y-textheight("x")/2, (char*)str2);
            settextstyle(0,HORIZ_DIR,marimeFont);
        }
 
    }
 
    if(esteOperand(a->info)){
 
        if(a->info.find("log_") != string::npos ){ // log_baza_argument /// log_24_35
            string str;
            str=a->info;
            const char* str1=str.c_str();
 
            outtextxy(c.x - (a->info.size()-2)*textwidth("x")/2,c.y-textheight("log")/2,"log");
            string baza, argument;
            int check=0;
            for(int i=4;i<=a->info.size();i++){
                if(check==1) argument.push_back(a->info[i]);
                if(a->info[i]!='_' && check==0)baza.push_back(a->info[i]);
                else if(check==0) check=1;
            }
            str=baza;
            str1=str.c_str();
            outtextxy(c.x-(a->info.size()-8)*textwidth("x")/2 ,c.y,(char*)str1);
 
            str=argument;
            str1=str.c_str();
            outtextxy(c.x -(a->info.size()-8-baza.size()*2)*textwidth("x")/2,c.y-textheight("X")/2,(char*)str1);
 
        }
        else if(a->info.find("_")!=string::npos){
            string ante_usc, post_usc;
            int check=0;
            for(int i=0;i<=a->info.size();i++){
                if(check==1) post_usc.push_back(a->info[i]);
                if(a->info[i]!='_' && check==0) ante_usc.push_back(a->info[i]);
                else if(check==0) check=1;
            }
            if(ante_usc.size()>post_usc.size()){
                const char* str1=ante_usc.c_str();
                outtextxy(c.x-textwidth("x")*(ante_usc.size()-1)/2-textwidth("x")*post_usc.size()/2, c.y-textheight("x")/2, (char*)str1);
                const char* str2=post_usc.c_str();
                outtextxy(c.x+textwidth("x")*(ante_usc.size()+1)/2-textwidth("x")*post_usc.size()/2, c.y, (char*)str2);
            }
            else{
                const char* str1=ante_usc.c_str();
                outtextxy(c.x-textwidth("x")*(ante_usc.size()+post_usc.size()-1)/2, c.y-textheight("x")/2, (char*)str1);
                const char* str2=post_usc.c_str();
                outtextxy(c.x-textwidth("x")*(post_usc.size()-1)/2 + textwidth("x")*(ante_usc.size())/2, c.y, (char*)str2);//+textwidth("x")*(post_usc.size())/2
            }
 
        }
        else if(a->info.find("pi")==0)
            pi(c.x,c.y,textheight("x"));
        else{
            string str;
            str=a->info;
            const char* str1=str.c_str();
            outtextxy(c.x-textwidth((char*)str1)/2,c.y-textheight((char*)str1)/2,(char*)str1);
            }
        settextstyle(0,HORIZ_DIR,marimeFont);
    }
    else if(a->info=="^"){//meeds rework
        string str;
        str=a->info;
        const char* str1=str.c_str();
 
        Cursor c1,c2;
        c1.x=c.x; c1.y=c.y;
 
        int txtw=textwidth((char*)str1);
 
        settextstyle(0,HORIZ_DIR,marimeFont-1);
        if(a->dreapta->info=="/")
            c2.x=c.x+lungimeComponenta(a->stanga)*txtw/2 + lungimeComponenta(a->dreapta)*textwidth((char*)str1)/2-textwidth("X")/2;
        else c2.x=c.x+lungimeComponenta(a->stanga)*txtw/2 + lungimeComponenta(a->dreapta)*textwidth((char*)str1)/2+ compensareOBt(a->dreapta)/2;//
        c2.y=c.y-latimeFractieSus(a->dreapta)*textheight("X")/2;
 
        settextstyle(0,HORIZ_DIR,marimeFont);
 
        if(esteOperator(a->stanga->info)){
            c1.x =c.x + compensareOBt(a->stanga)/2 - textwidth(")");
            outtextxy(c1.x-lungimeComponenta(a->stanga->stanga)*textwidth("x")-textwidth("(")*6/4,c1.y-textheight(")")/2,"(");
            outtextxy(c1.x+textwidth(")")/2+lungimeComponenta(a->stanga->dreapta)*textwidth("x"),c1.y-textheight(")")/2,")");
        }
 
        settextstyle(0,HORIZ_DIR,marimeFont);
        draw(a->stanga,c1, marimeFont);
        settextstyle(0,HORIZ_DIR,marimeFont-1);
        draw(a->dreapta,c2, marimeFont-1);
        settextstyle(0,HORIZ_DIR,marimeFont);
    }
    else if(a->info=="+" || a->info=="-" || a->info=="*"){
        string str;
        str=a->info;
        const char* str1=str.c_str();
 
        cazunic=true;
 
        Cursor stanga, dreapta;
        stanga.y=c.y; dreapta.y=c.y;
 
         if(a->stanga->info == "-" || a->stanga->info == "+" || a->stanga->info == "*"){///nu am habar de ce asta exista
            if(a->stanga->info=="*" && (a->stanga->dreapta->info=="+"||a->stanga->dreapta->info=="-") ) //|| /paranteze: a*(b-+c) sau (a-+b)*c si pe pozitii!
                stanga.x=c.x-(lungimeComponenta(a->stanga->dreapta)+3)*textwidth((char*)str1);
            else if(a->stanga->info=="-" && (a->stanga->dreapta->info=="-"||a->stanga->dreapta->info=="+"))
                stanga.x=c.x-(lungimeComponenta(a->stanga->dreapta)+1)*textwidth((char*)str1);
            else stanga.x=c.x-(lungimeComponenta(a->stanga->dreapta)+1)*textwidth((char*)str1);
 
            if(a->dreapta->info=="*" && (a->dreapta->dreapta->info=="-"||a->dreapta->dreapta->info=="+"))///paranteze
                dreapta.x=c.x+(lungimeComponenta(a->dreapta))*textwidth((char*)str1)/2+compensareOBt(a->dreapta)/2;
            else if(a->dreapta->info=="*" && (a->dreapta->stanga->info=="-"||a->dreapta->stanga->info=="+"))
                dreapta.x=c.x+(lungimeComponenta(a->dreapta)+2)*textwidth((char*)str1)/2+compensareOBt(a->dreapta)/2;///+2?
            else if(a->dreapta->info=="-" && a->dreapta->stanga->info=="0"){
                //cazunic=true;
                minusflag=true;
                dreapta.x=c.x+(lungimeComponenta(a->dreapta)+2)*textwidth((char*)str1)/2+compensareOBt(a->dreapta)/2;
                }
            else if(a->info=="-" && (a->dreapta->info=="-"||a->dreapta->info=="+"))
                dreapta.x=c.x+(lungimeComponenta(a->dreapta)+3)*textwidth((char*)str1)/2+compensareOBt(a->dreapta)/2;
            else if(a->dreapta->info=="!")
                dreapta.x=c.x+(lungimeComponenta(a->dreapta)+2)*textwidth((char*)str1)/2+compensareOBt(a->dreapta)/2;
            //else if(a->info=="-" && primulSemnEMinus(a->dreapta)) dreapta.x=c.x+(lungimeComponenta(a->dreapta)+3)*textwidth((char*)str1)/2+compensareOBt(a->dreapta)/2;
            //else if(a->info=="+" && primulSemnEMinus(a->dreapta)) dreapta.x=c.x+(lungimeComponenta(a->dreapta)-1)*textwidth((char*)str1)/2+compensareOBt(a->dreapta)/2;
            else dreapta.x=c.x+(lungimeComponenta(a->dreapta)+1)*textwidth((char*)str1)/2+compensareOBt(a->dreapta)/2;//cu skepsis
        }
        else {
            if(a->dreapta->info=="*" && (a->dreapta->dreapta->info=="-" || a->dreapta->dreapta->info=="+"))
                dreapta.x=c.x+(lungimeComponenta(a->dreapta))*textwidth((char*)str1)/2+compensareOBt(a->dreapta)/2;
            else if(a->dreapta->info=="*" && (a->dreapta->stanga->info=="-" || a->dreapta->stanga->info=="+"))
                dreapta.x=c.x+(lungimeComponenta(a->dreapta)+2)*textwidth((char*)str1)/2+compensareOBt(a->dreapta)/2;///+2?
            else if(a->info=="-" && primulSemnEMinus(a->dreapta))
                dreapta.x=c.x+(lungimeComponenta(a->dreapta)+1)*textwidth((char*)str1)/2+compensareOBt(a->dreapta)/2;//+2
            else if(a->dreapta->info=="^")
                dreapta.x=c.x+(lungimeComponenta(a->dreapta))*textwidth((char*)str1)/2;// ///+2+compensareOBt(a->dreapta)/2
            else if(a->dreapta->info=="!")
                dreapta.x=c.x+(lungimeComponenta(a->dreapta)+2)*textwidth((char*)str1)/2+compensareOBt(a->dreapta)/2;
            //else if(a->dreapta->info=="/")
             //   dreapta.x=c.x+(lungimeComponenta(a->dreapta))*textwidth((char*)str1)/2+compensareOBt(a->dreapta)/2;
            else dreapta.x=c.x+(lungimeComponenta(a->dreapta)+1)*textwidth((char*)str1)/2+compensareOBt(a->dreapta)/2;
 
            if(a->stanga->info=="abs")
                stanga.x=c.x-(lungimeComponenta(a->stanga)/2)*textwidth("x")-textwidth("|")/2;
            else if(a->stanga->info=="sqrt")
                stanga.x=c.x-(lungimeComponenta(a->stanga)/2)*textwidth("x")-textwidth("|")/2;
            else if(a->stanga->info=="!")
                stanga.x=c.x-(lungimeComponenta(a->stanga)/2)*textwidth("x");
            else if(esteOperatorUnar(a->stanga->info)) ///a->stanga->info!="abs" && a->stanga->info!="sqrt" &&
                stanga.x=c.x-(lungimeComponenta(a->stanga->unar)/2+a->stanga->info.size())*textwidth((char*)str1)-textwidth(".")/2+compensareOBt(a->stanga);//-compensareOBt(a->stanga->unar)/[+2?] foarte confuz cu sin[abs[x/3-4+abs[x]]]-1 /// /2 mi-e frica ca-l strica
            else if(a->stanga->info=="/")
                stanga.x=c.x-(lungimeComponenta(a->stanga)/2-1)*textwidth((char*)str1); ///s-ar putea sa strice
            else if(a->stanga->info=="^")
                stanga.x=c.x-(lungimeComponenta(a->stanga->dreapta))*textwidth((char*)str1)*8/10-lungimeComponenta(a->stanga->stanga)*textwidth("x")/2;//
            //else if(a->info=="-" && (a->dreapta->info=="-"||a->dreapta->info=="+"))
            //    stanga.x=c.x-(lungimeComponenta(a->stanga)/2+3)*textwidth((char*)str1);
            else stanga.x=c.x-(lungimeComponenta(a->stanga)/2+1)*textwidth((char*)str1);//+1
 
        }
 
        if(a->info == "*" && (a->stanga->info=="+" || a->stanga->info=="-")){ ///probabil gata
            stanga.x=stanga.x-textwidth(")");
            outtextxy(stanga.x-lungimeComponenta(a->stanga->dreapta)*textwidth("x")/2-textwidth("(|"),stanga.y-textheight(")")/2,"(");
            outtextxy(stanga.x+textwidth(")")/2+lungimeComponenta(a->stanga->dreapta)*textwidth("x"),stanga.y-textheight(")")/2,")");
        } else if(a->info == "*" && (a->dreapta->info=="+" || a->dreapta->info=="-")){ ///-x  || (a->info=="+"||a->info=="*")
            dreapta.x=dreapta.x+textwidth("(");
            outtextxy(dreapta.x-lungimeComponenta(a->dreapta->stanga)*textwidth("x")-textwidth("(XX")/2,dreapta.y-textheight(")")/2,"(");
            outtextxy(dreapta.x+textwidth(")")/2+lungimeComponenta(a->dreapta->dreapta)*textwidth("x"),dreapta.y-textheight(")")/2,")");
        } else if((a->info=="-" || a->dreapta->indice_nod_special!=-1) && (a->dreapta->info=="+" || a->dreapta->info=="-")) {//&& primulSemnEMinus(a->dreapta
            outtextxy(dreapta.x-lungimeComponenta(a->dreapta->stanga)*textwidth("x")-textwidth("(XX")/2,dreapta.y-textheight(")")/2,"(");
            outtextxy(dreapta.x+textwidth(")")/2+lungimeComponenta(a->dreapta->dreapta)*textwidth("x"),dreapta.y-textheight(")")/2,")");
        }
 
        if(a->info=="*" && esteNumar(a->stanga->info) && esteOperand(a->dreapta->info) && !esteNumar(a->dreapta->info))
            stanga.x+= 3*textwidth("x")/4;
        else if(a->info=="*") {
            setfillstyle(SOLID_FILL,setari.culoare1);
            circle(c.x, c.y, 4);
            floodfill(c.x,c.y,setari.culoare1);
        }
        else outtextxy(c.x-textwidth((char*)str1)/2,c.y-textheight((char*)str1)/2,(char*)str1);
 
        if(a->dreapta->info=="/")
            dreapta.x-=compensareOBt(a->dreapta)/2;
        if(a->stanga->info=="/")
            stanga.x-=textwidth("|");
 
        draw(a->dreapta, dreapta, marimeFont);
        if(a->info=="-" && a->stanga->info=="0"){//&& (esteOperand(a->dreapta->info) ||esteOperatorUnar(a-?)
            if(minusflag==true){ ///s-ar putea sa fie foarte instabil, draw la dreapta mutat sus pare ca rezolva, dar mai trebuie teste
            outtextxy(c.x-4*textwidth("x")/3, c.y-textheight("x")/2,"(");
            outtextxy(c.x+ (lungimeComponenta(a->dreapta))*textwidth("x")+textwidth("x")/2,c.y-textheight("x")/2,")");
            minusflag=false;
            }
        }
        else draw(a->stanga, stanga, marimeFont);
 
        cazunic=false;
    }
    else if(a->info=="/"){ ///trebuie neaparat o alternativa pentru scalabilitate
        Cursor sus, jos;
        if(a->stanga->info=="/")
            sus.x=c.x;
        else if(a->stanga->info=="^"){
            if(lungimeComponentaC(a->stanga->stanga)<lungimeComponentaC(a->stanga->dreapta))
                sus.x=c.x+compensareOBt(a->stanga)/2;
            else sus.x=c.x;
        }
        else if(a->stanga->info=="!")
            sus.x=c.x+textwidth("!");
        else sus.x=c.x+compensareOBt(a->stanga)/2;
 
        if(a->dreapta->info=="/") //|| a->dreapta->info=="^"&& esteOperatorUnar(a->dreapta->stanga->info)
            jos.x=c.x;
        else if(a->dreapta->info=="^"){
            if(lungimeComponentaC(a->dreapta->stanga)<lungimeComponentaC(a->dreapta->dreapta))
                jos.x=c.x+compensareOBt(a->dreapta)/2;//
            else jos.x=c.x;
        }
        else jos.x=c.x+compensareOBt(a->dreapta)/2;
 
        if(esteOperand(a->stanga->info)&&a->stanga->info!="^")//+textheight("x")/3//if(esteOperator(a->stanga->info)||esteop)
            sus.y=c.y-textheight("X")/2;
        else sus.y=c.y-(latimeFractieSus(a->stanga))*textheight("x")/2;//sus.y=c.y-(latimeFractieSus(a->stanga)/2+1)*textheight("x");
        draw(a->stanga, sus, marimeFont);
 
        if(esteOperand(a->dreapta->info)&&a->dreapta->info!="^")
            jos.y=c.y+textheight("X")/2;
        else jos.y=c.y+(latimeFractieJos(a->dreapta))*textheight("x")/2;//+textheight("x")/2
        draw(a->dreapta, jos, marimeFont);
 
        line(c.x-max(lungimeComponenta(a->stanga),lungimeComponenta(a->dreapta))*textwidth("x")/2-1,c.y,c.x+max(lungimeComponenta(a->stanga),lungimeComponenta(a->dreapta))*textwidth("x")/2+1,c.y);
    }
    else if(a->info=="sqrt"){
        Cursor cr;
        cr.x=c.x; cr.y=c.y;
 
        int aux=cr.x;
        if(a->unar->info=="/");
        else cr.x+= compensareOBt(a->unar)/2;
 
        draw(a->unar, cr, marimeFont);
        radicalC(aux,cr.y,lungimeComponenta(a->unar),latimeComponenta(a->unar));
    }
    else if(a->info=="abs"){
        Cursor cr;
        cr.x=c.x; cr.y=c.y;
 
        line(cr.x-lungimeComponenta(a->unar)*textwidth("x")/2-3, cr.y-latimeComponenta(a->unar)*textwidth("x")/2-10, cr.x-lungimeComponenta(a->unar)*textwidth("x")/2-3, cr.y+latimeComponenta(a->unar)*textwidth("x")/2+10);
        line(cr.x+lungimeComponenta(a->unar)*textwidth("x")/2+3, cr.y-latimeComponenta(a->unar)*textwidth("x")/2-10, cr.x+lungimeComponenta(a->unar)*textwidth("x")/2+3, cr.y+latimeComponenta(a->unar)*textwidth("x")/2+10);
 
        ///compensare pentru functii, daca nu, aia e
        if(a->unar->info=="^") cr.x=cr.x- lungimeComponenta(a->unar->dreapta)*textwidth("x")/2;
        else if(a->unar->info=="/");
        else cr.x+=compensareOBt(a->unar)/2; ///nu mai stiu de ce compensez
        draw(a->unar, cr, marimeFont);
    }
    else if(esteOperatorUnar(a->info) && a->info!="sqrt" && a->info!= "abs" && a->info!="!"){
        string str;
        str=a->info;
        const char* str1=str.c_str();
 
        Cursor stg, dr;
        stg.y=c.y; dr.y=c.y;
        dr.x=c.x;
 
        stg.x=c.x-lungimeComponenta(a)*textwidth("(")/2+textwidth((char*)str1)/2; //cursorul stg pune denumirea functiei si paranteza de inceput
 
        if(a->info.size()<=3){
            if(esteOperand(a->unar->info))
                dr.x=stg.x+(lungimeComponenta(a->unar)/2+a->info.size())*textwidth("-")+(1-a->info.size()%2)*textwidth("x")/2+compensareOBt(a->unar) ; //cursorul dreapta pozitioneaza argumentul
            else if(esteOperatorUnar(a->unar->info) && a->unar->info.size()>3)
                dr.x=stg.x+(lungimeComponenta(a->unar->unar)/2+a->info.size()+2*a->unar->info.size()/3)*textwidth("-");
            else if(esteOperatorUnar(a->unar->info) && a->unar->info!="abs")
                dr.x=stg.x+(lungimeComponenta(a->unar->unar)/2+a->info.size()+2*a->unar->info.size()/3+1)*textwidth("-");///heavely modded //-1//valabil doar pentru sin/cos //de ce +5? ///de ce +2 cu +4?
 
            else if(a->unar->info=="abs")
                dr.x=stg.x+(lungimeComponenta(a->unar->unar)/2+2)*textwidth("x")+textwidth("|");
            else if(a->unar->info=="/")
                dr.x=stg.x+(lungimeComponenta(a->unar)/2+a->info.size()/2)*textwidth("-")+3*textwidth("-")/2;
            else dr.x=stg.x+(lungimeComponenta(a->unar->stanga)+a->info.size())*textwidth("-");
 
            outtextxy(stg.x+textwidth((char*)str1)/2 ,stg.y-textheight("(")/2,"(");
            outtextxy(stg.x-textwidth((char*)str1)/2,stg.y-textheight((char*)str1)/2,(char*)str1);
 
            if(esteOperand(a->unar->info))
                outtextxy(dr.x + lungimeComponenta(a->unar)*textwidth(")")/2, stg.y-textheight(")")/2, ")");
            else if(esteOperatorUnar(a->unar->info) && a->unar->info!="abs")
                outtextxy(dr.x+(lungimeComponenta(a->unar))*textwidth("x")/2, stg.y-textheight(")")/2, ")");///->unaraici era un ->unar->unar, de ce? ///skepsis major
            else if(a->unar->info=="abs")
                outtextxy(dr.x + (lungimeComponenta(a->unar->unar))*textwidth("x")/2, stg.y-textheight(")")/2, ")");//+textwidth(")")
            else if(a->unar->info=="/")
                outtextxy(dr.x + lungimeComponenta(a->unar)*textwidth("-")/2, stg.y-textheight(")")/2, ")");
            else if(a->unar->dreapta!=NULL && esteOperand(a->unar->dreapta->info))
                outtextxy(dr.x + a->unar->dreapta->info.size()*textwidth("x")+textwidth(")")/2, stg.y-textheight(")")/2, ")");
            else outtextxy(dr.x + lungimeComponenta(a->unar->dreapta)*textwidth("x")+textwidth(")"), stg.y-textheight(")")/2, ")");
        }
        else{
            if(esteOperand(a->unar->info))
                dr.x=stg.x+(lungimeComponenta(a->unar)/2+a->info.size())*textwidth("-")-(a->info.size()%5+2)*textwidth("-")/2+compensareOBt(a->unar); //cursorul dreapta pozitioneaza argumentul
            else if(esteOperatorUnar(a->unar->info) && a->unar->info!="abs")
                dr.x=stg.x+(lungimeComponenta(a->unar->unar)/2+a->info.size()+2*a->unar->info.size()/3+1)*textwidth("-");///heavely modded //-1//valabil doar pentru sin/cos //de ce +5? ///de ce +2 cu +4?
            else if(a->unar->info=="abs")
                dr.x=stg.x+(lungimeComponenta(a->unar->unar)/2+2)*textwidth("x")+textwidth("|");
            else if(a->unar->info=="/")
                dr.x=stg.x+(lungimeComponenta(a->unar)/2+a->info.size()/2+1)*textwidth("-");
            else dr.x=stg.x+(lungimeComponenta(a->unar->stanga)+a->info.size()-2)*textwidth("-")+12*textwidth("-")/10 - a->info.size()/6*textwidth("-")/2;//
 
            outtextxy(stg.x+textwidth((char*)str1)/2 ,stg.y-textheight("(")/2,"(");
            outtextxy(stg.x-textwidth((char*)str1)/2,stg.y-textheight((char*)str1)/2,(char*)str1);
 
            if(esteOperand(a->unar->info))
                outtextxy(dr.x + lungimeComponenta(a->unar)*textwidth(")")/2, stg.y-textheight(")")/2, ")");
            else if(esteOperatorUnar(a->unar->info) && a->unar->info!="abs")
                outtextxy(dr.x+(lungimeComponenta(a->unar->unar)+5)*textwidth("x")/2, stg.y-textheight(")")/2, ")");///aici era un ->unar->unar, de ce? ///skepsis major
            else if(a->unar->info=="abs")
                outtextxy(dr.x + (lungimeComponenta(a->unar->unar))*textwidth("x")/2, stg.y-textheight(")")/2, ")");//+textwidth(")")
            else if(a->unar->info=="/")
                outtextxy(dr.x + lungimeComponenta(a->unar)*textwidth("-")/2, stg.y-textheight(")")/2, ")");
            else if(a->unar->dreapta!=NULL && esteOperand(a->unar->dreapta->info))
                outtextxy(dr.x + a->unar->dreapta->info.size()*textwidth("x")+textwidth(")")/2, stg.y-textheight(")")/2, ")");
            else outtextxy(dr.x + lungimeComponenta(a->unar->dreapta)*textwidth("x"), stg.y-textheight(")")/2, ")");
        }
 
        draw(a->unar, dr, marimeFont);
    }
    else if(a->info=="!"){
        Cursor stg;
        stg.y=c.y;
 
        if(esteOperator(a->unar->info) && a->unar->info!="/" || esteOperatorUnar(a->unar->info)){
            stg.x=c.x-3*textwidth("x")/10-lungimeComponenta(a->unar)/2+compensareOBt(a->unar)/2;//
            outtextxy(c.x+(lungimeComponenta(a->unar)-1)*textwidth("x")/2, c.y-textheight("!")/2, ")!");
            if(a->unar->info=="*")
                outtextxy(stg.x-(lungimeComponenta(a->unar->stanga)+1)*textwidth("x"),c.y-textheight("!")/2, "(");//ajustari minore
            else outtextxy(stg.x-(lungimeComponenta(a->unar->stanga))*textwidth("x")-4*textwidth("x")/3,c.y-textheight("!")/2, "(");
        }
        else if(esteOperand(a->unar->info) || a->unar->info=="/"){
            stg.x=c.x-textwidth("x");
            outtextxy(c.x+(lungimeComponenta(a->unar)-2)*textwidth("x")/2, c.y-textheight("!")/2, "!");//-2 pare sa fie periculos
        }
 
        draw(a->unar, stg, marimeFont);
    }
 
}
 
void determinareDimensiuni(){
    settextstyle(0,HORIZ_DIR,coeficientScala);
    lungimeEcran=(lungimeComponenta(ArbConstruit)+contor_vector_indici)*textwidth("x")+300;
    latimeEcran=latimeComponenta(ArbConstruit)*textheight("x")+150;
    coeficientScala -= (lungimeEcran>getmaxwidth() ? ((lungimeEcran+getmaxwidth()/2+getmaxwidth()/3)/getmaxwidth()) : 0);
}
 
void desenareFormula(){
    determinareDimensiuni();
 
    initwindow(lungimeEcran,latimeEcran,"FORMULA",100,100);
    setfillstyle(SOLID_FILL,setari.culoare2);
    setcolor(setari.culoare1);
    setbkcolor(setari.culoare2);
 
    floodfill(1,1,setari.culoare2);
    Cursor c;
    c.x= (lungimeEcran>getmaxwidth() ? getmaxwidth()/2 : lungimeEcran/2);
    c.y=latimeEcran/2;
 
    settextstyle(0,HORIZ_DIR,coeficientScala);
 
    if(esteOperand(ArbConstruit->info));
    else if(esteOperatorUnar(ArbConstruit->info)) ///merge pe majoritatea cazurilor
        c.x=c.x - compensareOBt(ArbConstruit); //cazul cand am ca radacina un operator unar, nu ma pot duce in stanga/dreapta lui ca mai jos
    else {
        if(ArbConstruit->info!="/" && (lungimeComponenta(ArbConstruit->stanga) < lungimeComponenta(ArbConstruit->dreapta))) ///de ce aici era >?
            c.x=c.x+(lungimeComponenta(ArbConstruit->stanga)-lungimeComponenta(ArbConstruit->dreapta))*textwidth("x")/2;
        else if(ArbConstruit->info=="/");
        else c.x=c.x-(lungimeComponenta(ArbConstruit->dreapta)-lungimeComponenta(ArbConstruit->stanga)+1)*textwidth("x")/2;
    }
 
    //if(ArbConstruit->info=="/") ///prototip
     //   if(lungimeComponenta(ArbConstruit->stanga) > lungimeComponenta(ArbConstruit->dreapta)) c.y=c.y+lungimeComponenta(ArbConstruit->stanga)*textheight("X")/3;
     //   else c.y=c.y-lungimeComponenta(ArbConstruit->stanga)*textheight("X")/3;
 
    draw(ArbConstruit, c, coeficientScala);
 
    if(setari.salvareFormulaInPoza==true) {
        string str;
        str=std::to_string(setari.nr_imagini);
        str.append(".bmp");
        const char* str1=str.c_str();
        writeimagefile((char*)str1);
        cout<<"Imagine salvata!"<<endl;
        setari.nr_imagini++;
        proceduraSalvareSetari();
    }
 
    getch(); closegraph();
}
 
 
/// DESENAREA ARBORELUI ///
 
int nrNiveluri(arbore *a){
    if(a==NULL)return 0;
    else
    {
        int n1=nrNiveluri(a->stanga);
        int n2=nrNiveluri(a->dreapta);
        int n3=nrNiveluri(a->unar);
        return 1+max(max(n1,n2),n3);
    }
}
int nrColoane(arbore *a){
 
    if(a==NULL)return 0;
    else{
            int n1=nrColoane(a->stanga)+1;
            int n2=nrColoane(a->dreapta)+1;
            int n3=nrColoane(a->unar)-nrColoane(a->unar)/2;
            return 1+n1+n2+n3;
        }
}
 
void deseneazaNod(string elem, int xc, int yc, int latime, int inaltime){
    settextstyle(BOLD_FONT ,HORIZ_DIR,2);
    int x1=xc-textwidth((char*)elem.c_str())/2-10;
    int y1=yc-textheight((char*)elem.c_str())/2-5;
    int x2=xc+textwidth((char*)elem.c_str())/2+10;
    int y2=yc+textheight((char*)elem.c_str())/2+5;
 
    setfillstyle(SOLID_FILL,RED);
    rectangle(x1,y1,x2,y2);
    floodfill(xc,yc,WHITE);
    setbkcolor(RED);
 
    settextjustify(1,1);
    outtextxy(xc,yc+5,(char*)elem.c_str());
    setcolor(WHITE);
}
void deseneazaArbore(arbore *a, int niv, int stanga, int latime, int inaltime){
 
    if(a!=NULL){
        int n1=nrColoane(a->stanga);
        int xc=stanga+latime*n1+latime/2;
        int yc=niv*inaltime-inaltime/2;
 
        if (a->stanga!=NULL){
            int xcs=stanga+latime*nrColoane(a->stanga->stanga)+latime/2;
            setcolor(LIGHTGRAY);
            line(xc,yc,xcs,yc+inaltime);
            setcolor(WHITE);
        }
        if (a->dreapta!=NULL){
            int xcd=stanga+latime*(n1+1)+latime*nrColoane(a->dreapta->stanga)+latime/2;
            setcolor(LIGHTGRAY);
            line(xc,yc,xcd,yc+inaltime);
            setcolor(WHITE);
        }
        if (a->unar!=NULL){
            int xcm=stanga+latime*n1+latime*nrColoane(a->unar->stanga)+latime/2;
            setcolor(LIGHTGRAY);
            line(xc,yc,xcm,yc+inaltime);
            setcolor(WHITE);
 
        }
        if (a->indice_nod_special!=-1){
            const char* str1;
            if(arr_indici_noduri_speciale[a->indice_nod_special].tip==1) str1="SUMA";
            if(arr_indici_noduri_speciale[a->indice_nod_special].tip==2) str1="produs";
            if(arr_indici_noduri_speciale[a->indice_nod_special].tip==3) str1="integrala nedef";
            if(arr_indici_noduri_speciale[a->indice_nod_special].tip==4) str1="integrala def";
            buton(xc, yc-50, (char*)str1, RED);
        }
 
        deseneazaNod(a->info,xc,yc,latime,inaltime);
        deseneazaArbore(a->unar,niv+1,stanga,latime,inaltime);
        deseneazaArbore(a->stanga,niv+1,stanga,latime, inaltime);
        deseneazaArbore(a->dreapta,niv+1,stanga+latime*(n1+1), latime, inaltime);
 
    }
}
 
void FereastraDesenareArbore(arbore *a){
    int inaltime_F=800, latime_F=1300; //dimensiuni fereastra
 
    initwindow(latime_F,inaltime_F,"ARBORELE FORMULEI", 200,200);
    setcolor(WHITE); setbkcolor(LIGHTRED); cleardevice();
    rectangle(1,1,latime_F-1,inaltime_F-1);
 
    int latime_A, inaltime_A;             //dimensiuni arbore
    latime_A=inaltime_F/nrColoane(a)+5;///aici nu era +5
    inaltime_A=inaltime_F/nrNiveluri(a);
 
    deseneazaArbore(a,1,0,latime_A,inaltime_A);
 
    getch();
    closegraph();
}
 
/// DESENAREA MENIULUI PRINCIPAL///
 
 
/** MANAGEMENTUL ISTORICULUI **/
 
void introducereIstoric(string sirDeIntrodus){
     FILE* fisierscriere;
    if( (fisierscriere = fopen("istoricformule.txt", "a")) == NULL){
        cout<<"Eroare la deschiderea fisierului cu istoricul formulelor"<<endl;
        exit(1);
    }
    else{
        istoricformule[nrElementeIstoric++]=sirDeIntrodus;
        const char* str1=sirDeIntrodus.c_str();
        fprintf(fisierscriere,"\n");
        fprintf(fisierscriere,"%s",str1);
        fclose(fisierscriere);
    }
}
void citireIstoricDinFisier(){
    FILE* fisiercitire;
 
    char s[250];
    //nrElementeIstoric=0;
 
    if( (fisiercitire = fopen("istoricformule.txt", "r")) == NULL){
        cout<<"Eroare la deschiderea fisierului cu istoricul formulelor"<<endl;
    }
    else{
        while (fscanf(fisiercitire, "%[^\n] ", s) != EOF)
            istoricformule[nrElementeIstoric++].append(s);
        if(nrElementeIstoric==0){
            cout<<"Istoricul este gol"<<endl;
            nrElementeIstoric=0;
        }
        fclose(fisiercitire);
    }
}
void stergereIstoricDinFisier(bool toate){
    FILE* fisierstergere;
    if( (fisierstergere = fopen("istoricformule.txt", "w")) == NULL){
        cout<<"Eroare la deschiderea fisierului cu istoricul formulelor"<<endl;
        exit(1);
    }
    else{
        if(!toate)
            for(int i=0;i<nrElementeIstoric;i++){
                const char* str1=istoricformule[i].c_str();
                if(i<nrElementeIstoric-1)
                    fprintf(fisierstergere,"%s\n",str1);
                else
                    fprintf(fisierstergere,"%s",str1);///daca las simplu \ n va introduce un enter la final, dar fct care intrudce elementul va pune si ea un enter, stricand istoricul
 
            }
        else fprintf(fisierstergere,"");
 
        fclose(fisierstergere);
    }
}
int istoricPicker(){
 
    //imi iau dimensiunile ca sa pot crea o fereastra dinamica
    settextstyle(0,HORIZ_DIR,4);
    int txw=textwidth("X");//nu conteaza ce litera am pus acolo, toate caracterele au aceleasi dimensiuni
    int txh=textheight("X");
 
    //istoricFormule* istoric;
 
    if(istoricformule[0].empty()) citireIstoricDinFisier();
 
    bool gol=false;
    if(nrElementeIstoric==0) gol=true;
 
    int lungimemaxima=0;///lungimemaxima
    for(int i=0;i<nrElementeIstoric;i++)
        if(istoricformule[i].size()>lungimemaxima) lungimemaxima=istoricformule[i].size(); //iterez ca sa iau lungimea maxima de caractere din sirul din istoric [nu are protectie pentru formule lungi]
 
    if(gol==true) lungimemaxima=25;
    initwindow((lungimemaxima*txw>1800 ? 1800 : lungimemaxima*txw)+100, (nrElementeIstoric>10 ? 20 : nrElementeIstoric*2)*txh +200);///width-ul este 20 in mod normal [o linie contine o formula, urmatoarea nu] ca sa faca maxim 10 linii afisabile
 
    settextstyle(0,HORIZ_DIR,4);
 
    int k=0;
    ///go to este extrem de utila aici, ca simplu se duce sus, fara logica unui do while
 
    redesenareListaIstoric:
 
    cleardevice(); //sterg ecranul ca sa redesenez
    if(nrElementeIstoric==0) gol=true;
    if(gol==true) outtextxy(150, 150, "Istoricul este gol!");
 
    outtextxy(50, txh,"Istoric:");
 
    if(!gol)
    for(int i=0;i<(nrElementeIstoric>=10 ? 10 : nrElementeIstoric);i++){
        string str;
        str=istoricformule[i+k*10];//conversia uzuala, dar folosesc k pentru numarul "paginii", by default k=0 ca sunt pe pagina 0, apoi k=1 si afiseaza elementele de la 10 incolo
        const char* str1=str.c_str();
 
        if(str.size()*textwidth("x")>1900) settextstyle(0,HORIZ_DIR,3);
        outtextxy(50, txh*(i*2+6),(char*)str1);
        settextstyle(0,HORIZ_DIR,4);
    }
 
    outtextxy(getwindowwidth()-textwidth("Inapoi")-20,textheight("Inapoi"), "Inapoi");
    outtextxy(getwindowwidth()-textwidth("Sterge x1")-20,textheight("Inapoi")*2, "Sterge x1");
    outtextxy(getwindowwidth()-textwidth("Sterge tot")-20,textheight("Inapoi")*3, "Sterge tot");
    settextstyle(0,HORIZ_DIR,7);
    outtextxy(getwindowwidth()/2-textwidth("<")-40, txh+textheight("<"), "<"); //pentru niste butoane < > mai mari
    outtextxy(getwindowwidth()/2+textwidth(">")-20, txh+textheight(">"), ">");
    settextstyle(0,HORIZ_DIR,4);
 
 
    int x,y;
 
        do{
            if(ismouseclick(WM_LBUTTONDOWN)){
                clearmouseclick(WM_LBUTTONDOWN);
                x=mousex();
                y=mousey();
 
                for(int i=0 ;i< (nrElementeIstoric>=10 ? 10: nrElementeIstoric);i++)
                if(x>50 && x<(txw*istoricformule[i+k*10].size()+50) && y>txh*(i*2+6) && y<txh*(i*2+7)){
                        //closegraph();
                        return i+k*10; //aici verific fiecare linie daca e clickuita pe ea
                }
 
                if((x>getwindowwidth()-textwidth("Inapoi")-20) && (x>getwindowwidth()-textwidth("Inapoi")*2-20) && y>textheight("Inapoi") && y<textheight("Inapoi")*2) goto exit; //verific daca e clickuit butonul de inapoi
 
                if((x< getwindowwidth()/2-textwidth("<")) && (x>getwindowwidth()/2-textwidth("<")*3) && (y>txh+textheight("<")*2) && (y<txh+textheight("<")*3.5)){
                    if(k>0) { ///aici verific pe ce pagina sunt ca sa pot merge pe pagina de dinainte, daca sunt pe prima pagina nu va face nimic
                        k--;
                        goto redesenareListaIstoric;
                    }
                }
                if((x> getwindowwidth()/2+textwidth("<")) && (x<getwindowwidth()/2+textwidth("<")*3) && (y>txh+textheight("<")*2) && (y<txh+textheight("<")*3.5)){
                    if(k<nrElementeIstoric/10){
                        k++;
                        goto redesenareListaIstoric;
                    }
                }
 
                if((x>getwindowwidth()-textwidth("Sterge x1")-20) && (x>getwindowwidth()-textwidth("Sterge x1")*2-20) && y>textheight("X")*2 && y<textheight("X")*3)
                    {
                        cout<<"A fost sters elementul: "<<istoricformule[--nrElementeIstoric]<<endl;
                        istoricformule[nrElementeIstoric].clear();
                        stergereIstoricDinFisier(false);
                        goto redesenareListaIstoric;
                    }
                if((x>getwindowwidth()-textwidth("Sterge tot")-20) && (x>getwindowwidth()-textwidth("Sterge tot")*2-20) && y>textheight("X")*3 && y<textheight("X")*4)
                    {
                        cout<<"ISTORICUL A FOST GOLIT!"<<endl;
                        stergereIstoricDinFisier(true);
                        nrElementeIstoric=0;
                        goto redesenareListaIstoric;
                    }
            }
        }while(1);
 
    exit:
    closegraph();
    return -1;
}
 
 
/** CITIRE IN MOD GRAFIC **/
 
void CitesteSir(int x, int y, char mesaj[250], char S[250]){
 
    char Enter = 13;
    char BackSpace = 8;
    char Escape = 27;
    char s2[2]; s2[1]='\0';
    char MultimeDeCaractereAcceptabile[250]="0123456789abcdefghijklmnopqrstuvwxyz~!`@#$%+-^&*/\\()_=[],;.?<>:;{} ";
    char S_initial[250];
    char tasta='\0';
    char S_[250];
    char mesaj_[250];
 
    setcolor(WHITE);
    setbkcolor(LIGHTRED);
    cleardevice();
 
    strcpy(mesaj_,mesaj);
    strcpy(S_initial,S);
    settextstyle(BOLD_FONT ,HORIZ_DIR,1);
    setbkcolor(LIGHTCYAN);
    setcolor(MAGENTA);
 
 
    settextjustify(0,0);
    outtextxy(x,y,mesaj_);
    y=y+textheight(mesaj);
    strcpy(S,"");
    strcpy(S_,S);
    strcat(S_,"_");
    setcolor(LIGHTRED);
    setbkcolor(LIGHTRED);
    outtextxy(x,y,S_);
 
    s2[0]=tasta;
    strcat(S,s2);
    strcpy(S_,S);
    strcat(S_,"_");
    setcolor(WHITE);
    outtextxy(x,y,S_);
 
    do {
        tasta=getch();
        if (tasta==0)
        {
            tasta=getch();
            Beep(1000,500);
        }
        else
            if (strchr(MultimeDeCaractereAcceptabile, tasta))
            {
                // stergem fostul sir
                strcpy(S_,S);
                strcat(S_,"_");
                setcolor(LIGHTRED);
                setbkcolor(LIGHTRED);
                outtextxy(x,y,S_);
 
                s2[0]=tasta;
                strcat(S,s2);
                strcpy(S_,S);
                strcat(S_,"_");
                setcolor(WHITE);
                outtextxy(x,y,S_);
            }
            if (tasta==BackSpace)
                    if (!(strcmp(S,""))) Beep(500,100);
                    else
                    {
                        setcolor(LIGHTRED);
                        setbkcolor(LIGHTRED);
                        outtextxy(x,y,S_);
                        setcolor(WHITE);
                        S[strlen(S)-1]='\0';
                        strcpy(S_,S);
                        strcat(S_,"_") ;
                        outtextxy(x,y,S_);
                        Beep(200,20);
                    }
            else
                if (tasta!=Enter && tasta!=Escape)
                    {
                        Beep(200,20);
                    }
    } while (tasta!=Enter && tasta!=Escape);
    if (tasta == Escape)
        strcpy(S,S_initial);
 
    setcolor(LIGHTRED);
    setbkcolor(LIGHTRED);
    outtextxy(x,y,S_);
    setbkcolor(LIGHTCYAN);
    setcolor(MAGENTA);
    outtextxy(x,y,S);
}
 
void citireGrafica(){
  initwindow(1000,250,"Citire sir de caractere in mod grafic",200,200);
    char formula[250]="";
    char mesaj[250] = "Introduceti formula infixata: ";
 
    CitesteSir(10,70,mesaj,formula);
    outtextxy(10,150,"Ati introdus:");
    outtextxy(10,200,formula);
    formula_infixata.clear();
    formula_infixata.append(formula);
 
    if(setari.asteapta5secunde==false) getch();
    else{
        Sleep(1000);
        for(int i=5;i>=0;i--){
            char seInchideIn[50]="Fereastra se va inchide in ";
            char numar[2];
            sprintf(numar, "%d", i);
            strcat(seInchideIn, numar);
            strcat(seInchideIn, " secunde.");
            outtextxy(getwindowwidth()-textwidth("Fereastra se va inchide in x secunde")-20, 20,seInchideIn);
            Sleep(900);
        }
    }
 
    //closegraph();
}
 
    /** PROCEDURA AFISARE FORMULA **/
 
void proceduraAfisareFormula(){
    ///curatarea variabilelor globale
        for(int i=0;i<=nrMembri;i++)
            sirDeTokenuri[i].clear();
 
        nrMembri=0;
 
        for(int i=0;i<=nrTokenuri;i++)
            sirPostfixat[i].clear();
 
        nrTokenuri=0;
 
        coeficientScala = 5;
        minusflag=false;
        cazunic = false;
 
        ArbConstruit=NULL;
 
        contor_vector_indici=0;///probabil mai e nevoie si de sanitizare
 
    ///
 
    creareSirTokenizat();
 
    sanitizare();
    cout<<endl;
    cout <<"Formula tokenizata: ";
    for (int i = 0; i <= nrMembri; i++)
        cout << sirDeTokenuri[i] << " ";
    cout << endl;
 
    TokenizatLaPostfixat();
 
    cout <<"Formula postfixata: ";
    for (int i = 0; i <= nrMembri; i++)
        cout << sirPostfixat[i] << " ";
    cout << endl;
 
 
    ArbConstruit = construiesteArbore();
 
    atribNoduriSpec();
 
    inordine(ArbConstruit);
    cout<<endl;
    desenareFormula();
 
 
}
void proceduraAfisareFormulaFaraDesen(){
///curatarea variabilelor globale
        for(int i=0;i<=nrMembri;i++)
            sirDeTokenuri[i].clear();
 
        nrMembri=0;
 
        for(int i=0;i<=nrTokenuri;i++)
            sirPostfixat[i].clear();
 
        nrTokenuri=0;
 
        coeficientScala = 5;
        minusflag=false;
        cazunic = false;
 
        ArbConstruit=NULL;
 
        contor_vector_indici=0;///probabil mai e nevoie si de sanitizare
 
    ///
 
    creareSirTokenizat();
 
    sanitizare();
 
    TokenizatLaPostfixat();
 
    ArbConstruit = construiesteArbore();
 
    atribNoduriSpec();
}
void fereastraMeniu(){
 
    int x_buton_formula=250, y_buton_formula=80;
    int x_buton_afisare=250, y_buton_afisare=150;
    int x_buton_arbore=250, y_buton_arbore=220;
    int x_buton_istoric=250, y_buton_istoric=290;
    int x_buton_setari=250, y_buton_setari=360;
    int x_buton_exit=250, y_buton_exit=430;
 
    initwindow(500,500, "MENIU", getmaxwidth()/2-250, 250);//
    int x, y;
 
        setfillstyle(SOLID_FILL,12);     ///background
        floodfill(1,2,12);               ///background
 
    buton(x_buton_formula,y_buton_formula, "FORMULA", LIGHTMAGENTA);
    buton(x_buton_afisare,y_buton_afisare, "AFISARE", LIGHTGREEN);
    buton(x_buton_arbore,y_buton_arbore, "ARBORE", LIGHTMAGENTA);
    buton(x_buton_istoric,y_buton_istoric, "ISTORIC", LIGHTMAGENTA);
    buton(x_buton_setari,y_buton_setari, "SETARI", LIGHTMAGENTA);
    buton(x_buton_exit,y_buton_exit, "EXIT", LIGHTGREEN);
 
    //bool exit=false;
 
 
    while(1){
        if(ismouseclick(WM_LBUTTONDOWN)){
            clearmouseclick(WM_LBUTTONDOWN);
                x=mousex();
                y=mousey();
 
                if(x>(x_buton_formula-textwidth("FORMULA")-15)&&x<(x_buton_formula+textwidth("FORMULA")+15) && y>(y_buton_formula-textheight("FORMULA")-15)&&y<(y_buton_formula+textheight("FORMULA")+15))
                    {
                        cout<<"s a inregistrat un click pe formula"<<endl;
 
                        if(setari.metodaInput==1) citireGrafica();
                        else {
                            cout<<"Introduceti formula: ";
                            cin>>formula_infixata;
                        }
 
                        if(setari.salvareInIstoric==true) {
                                if(istoricformule[0].empty()) citireIstoricDinFisier();
                                introducereIstoric(formula_infixata);
 
                        }
 
                        if(setari.afisareDirecta==1)
                            proceduraAfisareFormula();
 
                        fereastraMeniu();
                    }
 
                if(x>x_buton_afisare-textwidth("AFISARE")-20 && x<x_buton_afisare+textwidth("AFISARE")+20 && y> y_buton_afisare-textheight("AFISARE")-15 &&y<y_buton_afisare+textheight("AFISARE")+15)
                    {
                        cout<<"s-a inregistrat un click pe afisare"<<endl;
                        if(formula_infixata.empty()) cout<<"NU S-A INTRODUS NICIO FORMULA!"<<endl;
                        else {
                            proceduraAfisareFormula();
                            fereastraMeniu();}
                    }
                if(x>x_buton_arbore-textwidth("ARBORE")-20 && x<x_buton_arbore+textwidth("ARBORE")+20 && y> y_buton_arbore-textheight("ARBORE")-15 &&y<y_buton_arbore+textheight("ARBORE")+15)
                    {
                        cout<<"s-a inregistrat un click pe arbore"<<endl;
                        if(ArbConstruit==NULL && formula_infixata.empty()) cout<<"NU S-A INTRODUS NICIO FORMULA!";
                        else {
                            proceduraAfisareFormulaFaraDesen();
                            FereastraDesenareArbore(ArbConstruit);
                            fereastraMeniu();
                        }
                    }
                 if(x>x_buton_istoric-textwidth("ISTORIC")-20 && x<x_buton_istoric+textwidth("ISTORIC")+20 && y> y_buton_istoric-textheight("ISTORIC")-15 &&y<y_buton_istoric+textheight("ISTORIC")+15)
                    {
                        cout<<"s-a inregistrat un click pe istoric"<<endl;
                        int indiceIstoric=istoricPicker();
 
                        if(indiceIstoric!=-1) formula_infixata=istoricformule[indiceIstoric];
 
                        if(setari.afisareDirecta==1 && indiceIstoric!=-1)
                            proceduraAfisareFormula();
 
                        fereastraMeniu();
                    }
                if(x>x_buton_setari-textwidth("SETARI")-20 && x<x_buton_setari+textwidth("SETARI")+20 && y> y_buton_setari-textheight("SETARI")-15 &&y<y_buton_setari+textheight("SETARI")+15)
                    {
                        cout<<"s-a inregistrat un click pe setari"<<endl;
                        fereastraSetari();
                        fereastraMeniu();
 
                    }
                if(x>x_buton_exit-textwidth("EXIT")-20 && x<x_buton_exit+textwidth("EXIT")+20 && y> y_buton_exit-textheight("EXIT")-15 &&y<y_buton_exit+textheight("EXIT")+15)
                    {
                        cout<<"s-a inregistrat un click pe exit"<<endl;
 
                        exit(1);
                    }
        }
    }
 
}
 
 
int main(){
    proceduraImportareSetari();
    fereastraMeniu();
}