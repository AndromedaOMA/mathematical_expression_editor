/*

Afisarea unei expresii matematice prin intermediul arborelui expresiei obtinut din notatia prefixata a acesteia.

Proiectul a fost dezvoltat pe parcursul a 3 iteratii:
1. In prima iteratie toate entitatile care formau expresia aveau lungimea 1 (numerele erau de fapt cifre,
variabilele erau formate dintr-un singur caracter etc.).
2. In a doua iteratie am rescris tipurile de date suport si functiile pentru a putea lucra cu operanzi
de lungime mai mare decat 1. Am pastrat si codul vechi (pentru operanzi de lungime 1) deoarece este nevoie
permanent sa ne testam noile functii (sa reparam eventualele bug-uri, de exemplu) prin compararea rezultatelor returnate
de acestea cu rezultatele stabile intoarse de functiile omonime din prima iteratie (introducand expresii
in care operanzii au lungime 1, aceste expresii vor functiona similar cu functiile din ambele iteratii).
3. In a treia iteratie am reparat multe dintre bug-urile de afisare a ecuatiei cu operanzi de lungime variabila
(spre exemplu bug-urile generate de afisarea fractiilor supraetajate, pozitionarea mai exacta
pe plansa a entitatilor din ecuatie etc.).

Proiectul este deschis pentru perfectionare si dezvoltare si contine inautrul sau (prin coexistenta
celor 3 iteratii) si parcursul urmat in urma feedback-ului primit la orele de curs si laborator.

*/
//#include <bits/stdc++.h>
#include <iostream>
#include <cstring>
#include <fstream>
#include "graphics.h"
//#pragma comment(lib,"graphics.lib")
using namespace std;

// -----------------------
// Constante, tipuri de date, structuri (toate globale)
// -----------------------

struct butonCuloare
{
	char nume[20];
	int culoare;
};

int index_culoareEcuatie = 0;
int index_culoareArbore = 0;
butonCuloare Culori[] = { {"Alb",WHITE}, {"Rosu Deschis",LIGHTRED},  {"Albastru Deschis",LIGHTBLUE},  {"Verde",GREEN},  {"Galben",YELLOW},  {"Mov Deschis",LIGHTMAGENTA} };

const int INFINIT = 1000000000;
const int NUMAR_ENTITATI = 100; // Indexam matricea de caractere de la linia 0
const int LUNGIME_ENTITATE = 20; // Inclusiv sfarsitul de sir de caractere

typedef char entitate[LUNGIME_ENTITATE];

const entitate _plus = "+";
const entitate _minus = "-";
const entitate _asterisc = "*";
const entitate _slash = "/";
const entitate _control = "^";
const entitate _parantezaRotundaDeschisa = "(";
const entitate _parantezaRotundaInchisa = ")";

const entitate _sin = "sin";
const entitate _cos = "cos";
const entitate _tg = "tg";
const entitate _ctg = "ctg";

struct matriceDeCaractere
{
	entitate tablouEntitati[NUMAR_ENTITATI];
	int dimensiuneTablouEntitati;
};

struct nodA
{
	char info;
	int linie, coloana; // Linia si coloana din plansa pe care va fi desenat nodul
	int numarColoaneLinieFractie; // Vom utiliza acest camp doar in cazul arbotilor care retin expresia de afisat, pentru nodurile care retin / (linia de fractie)
	nodA* st, * dr;
};
typedef nodA* arbore;

struct nodAE
{
	entitate info;
	int linie; // Linia pe care va fi desenat nodul
	float coloana; // Coloana pe care va fi desenat nodul (poate fi si fractionara petru iteratiile noi)
	int numarColoaneLinieFractie; // Vom utiliza acest camp doar in cazul arbotilor care retin expresia de afisat, pentru nodurile care retin / (linia de fractie retinuta si in constanta _slash)
	nodAE* st, * dr;
};
typedef nodAE* arboreE;

struct nod
{
	nod* urm;
	int info;
};
struct stiva
{
	nod* varf;
	int nrElem;
};

struct nodE // Nod entitate (pentru matrice de caractere)
{
	nodE* urm;
	entitate info;
};
struct stivaE // Stiva entitate (pentru matrice de caractere)
{
	nodE* varf;
	int nrElem;
};

/// VARIABILE "GLOBALE"


	// -----------------------
	// Variabile (iteratii vechi si noi)
	// -----------------------

arbore a = NULL, aExpresie = NULL;
arbore b = NULL, bExpresie = NULL;

arboreE _a = NULL, _aExpresie = NULL; // _a este arborle prefixului, _aExpresie este arborele expresiei
arboreE _b = NULL, _bExpresie = NULL; // _b este arborle prefixului parantezat, _bExpresie este arborele expresiei parantezate

int lungimePlansa = 900, inaltimePlansa = 600; // Comune pentru iteratii noi si vechi

int lungimeCelula, inaltimeCelula, razaCerc;
int lungimeCelulaArboreCuParanteze, inaltimeCelulaArboreCuParanteze, razaCercArboreCuParanteze;
int lungimeCelulaExpresie, inaltimeCelulaExpresie;
int T[10000], fiu_st[10000], fiu_dr[10000]; // Doar pentru iteratiile vechi
char arboreTablou[10000] = { 0 }; // Doar pentru iteratiile vechi
int dimensiuneTablouri; // Ultima pozitie ocupata din vectorul suport pentru arbore (doar pentru iteratiile vechi)

int _lungimeCelula, _inaltimeCelula, _razaCerc;
int _lungimeCelulaArboreCuParanteze, _inaltimeCelulaArboreCuParanteze, _razaCercArboreCuParanteze;
float _lungimeCelulaExpresie, _inaltimeCelulaExpresie; // real pentru o precizie mai buna
float _lungimeCelulaExpresieCuParanteze, _inaltimeCelulaExpresieCuParanteze; // real pentru o precizie mai buna (in iteratia 3)

string infix;
string postfix;
string prefix;

matriceDeCaractere _infix;
char infixString[NUMAR_ENTITATI * LUNGIME_ENTITATE];
matriceDeCaractere _postfix;
matriceDeCaractere _prefix;

int nrLiniiEcuatie, nrColoaneEcuatie;
int _nrLiniiEcuatie, _nrColoaneEcuatie;
int _nrLiniiEcuatieCuParanteze, _nrColoaneEcuatieCuParanteze; // In iteratia 3

int linieUrmatoareaFractie;
int _linieUrmatoareaFractie; // Nu se mai foloseste in iteratia 3

int minimNegativLinie, minimNegativColoana;
int maxPozitivLinie, maxPozitivColoana, numarLiniiPlansaEcuatie, numarColoanePlansaEcuatie;

int _minimNegativLinie;
float _minimNegativColoana; // Poate fi fractionar pentru iteratiile noi (2 sau 3)
int _maxPozitivLinie, _numarLiniiPlansaEcuatie, _numarColoanePlansaEcuatie;
float _maxPozitivColoana; // Poate fi fractionar pentru iteratiile noi (2 sau 3)

// In iteratia 3
int _minimNegativLinieEcuatieCuParanteze;
float _minimNegativColoanaEcuatieCuParanteze; // Poate fi fractionar pentru iteratiile noi
int _maxPozitivLinieEcuatieCuParanteze, _numarLiniiPlansaEcuatieCuParanteze, _numarColoanePlansaEcuatieCuParanteze;
float _maxPozitivColoanaEcuatieCuParanteze; // Poate fi fractionar pentru iteratiile noi

/// SFARSIT VARIABILE "GLOBALE


// -----------------------
// Functii pentru stiva
// -----------------------

void intializareStiva(stiva& s)
{
	s.varf = NULL;
	s.nrElem = 0;
}
void intializareStiva(stivaE& s)
{
	s.varf = NULL;
	s.nrElem = 0;
}
void push(stiva& s, int x)
{
	nod* nNou = new nod;
	nNou->info = x;
	nNou->urm = s.varf;
	s.varf = nNou;
	s.nrElem++;
}
void push(stivaE& s, const entitate x)
{
	nodE* nNou = new nodE;
	strcpy(nNou->info, x);
	nNou->urm = s.varf;
	s.varf = nNou;
	s.nrElem++;
}
void pop(stiva& s)
{
	nod* aux = s.varf;
	s.varf = s.varf->urm;
	delete aux;
	s.nrElem--;
}
void pop(stivaE& s)
{
	nodE* aux = s.varf;
	s.varf = s.varf->urm;
	delete aux;
	s.nrElem--;
}
int top(stiva s)
{
	return s.varf->info;
}
entitate* top(stivaE s)
{
	return &(s.varf->info);
}

// -----------------------
// Conversie din Infix in Prefix
// -----------------------

bool esteOperator(char c) // Operator sau paranteza
{
	return (!isalpha(c) && !isdigit(c)); // Este (, ), +, -, *, /, ^
}

/*   +   */
bool esteOperatorPlus(entitate c)
{
	return strcmp(c, _plus) == 0;
}

/*   -   */
bool esteOperatorMinus(entitate c)
{
	return strcmp(c, _minus) == 0;
}

/*   *   */
bool esteOperatorAsterisc(entitate c)
{
	return strcmp(c, _asterisc) == 0;
}

/*   \   */
bool esteOperatorSlash(entitate c)
{
	return strcmp(c, _slash) == 0;
}

/*   ^   */
bool esteOperatorControl(entitate c)
{
	return strcmp(c, _control) == 0;
}

bool esteOperator(entitate c)
{
	return esteOperatorPlus(c) || esteOperatorMinus(c) || esteOperatorAsterisc(c) || esteOperatorSlash(c) || esteOperatorControl(c);
}

/*   sin   */
bool esteFunctieSin(entitate c)
{
	return strcmp(c, _sin) == 0;
}

/*   cos   */
bool esteFunctieCos(entitate c)
{
	return strcmp(c, _cos) == 0;
}

/*   tg   */
bool esteFunctieTg(entitate c)
{
	return strcmp(c, _tg) == 0;
}

/*   ctg   */
bool esteFunctieCtg(entitate c)
{
	return strcmp(c, _ctg) == 0;
}

bool esteFunctie(entitate c)
{
	return esteFunctieSin(c) || esteFunctieCos(c) || esteFunctieTg(c) || esteFunctieCtg(c);
}

bool esteParantezaRotundaDeschisa(entitate c)
{
	return strcmp(c, _parantezaRotundaDeschisa) == 0;
}

bool esteParantezaRotundaInchisa(entitate c)
{
	return strcmp(c, _parantezaRotundaInchisa) == 0;
}

bool esteParanteza(entitate c)
{
	return esteParantezaRotundaDeschisa(c) || esteParantezaRotundaInchisa(c);
}

bool esteOperand(entitate c)
{
	return !esteOperator(c) && !esteParanteza(c) && !esteFunctie(c);
}

int prioritate(char c)
{
	if (c == '-' || c == '+')
		return 1;
	else if (c == '*' || c == '/')
		return 2;
	else if (c == '^')
		return 3;
	return 0;
}

int prioritate(entitate c)
{
	if (esteOperatorPlus(c) || esteOperatorMinus(c))
		return 1;
	else if (esteOperatorAsterisc(c) || esteOperatorSlash(c))
		return 2;
	else if (esteOperatorControl(c))
		return 3;
	return 0;
}

void ecuatieStringCatreTablouDeEntitati(char ecuatieString[], matriceDeCaractere& infix)
{
	// Daca ecuatia este (23+abc)/1331
	// atunci tabloul de entitati este {"(", "23", "+", "abc", ")", "/", "1331"}
	int i, j;
	char entitate[LUNGIME_ENTITATE];
	infix.dimensiuneTablouEntitati = 0;
	for (i = 0; i < strlen(ecuatieString); i++)
	{
		j = i;
		//while (j < strlen(ecuatieString) && !esteOperator(ecuatieString[j])) // Caut urmatorul operator sau paranteza cu varianta de functie pentru iteratiile vechi
		while (j < strlen(ecuatieString) && !strchr("()+-*/^", ecuatieString[j]))
			j++;
		if (i == j) // ecuatieString[i] este operator
		{
			infix.tablouEntitati[infix.dimensiuneTablouEntitati][0] = ecuatieString[i];
			infix.tablouEntitati[infix.dimensiuneTablouEntitati][1] = '\0';
			// infix.tablouEntitati[infix.dimensiuneTablouEntitati] are una dintre valorile "(", ")", "+", "-", "/", "*" sau "^"
		}
		else
		{
			strncpy(infix.tablouEntitati[infix.dimensiuneTablouEntitati], ecuatieString + i, j - i);
			infix.tablouEntitati[infix.dimensiuneTablouEntitati][j - i] = '\0';
			i = j - 1; // aceasta si i++ din for il sare pe i la inceputul urmatoarei entitati (operatorul sau paranteza de dupa operand)
		}
		infix.dimensiuneTablouEntitati++;
	}
}

string infixCatrePostfix(string infix)
{
	infix = '(' + infix + ')';
	int l = infix.size();
	stiva s;
	intializareStiva(s);
	string output;

	for (int i = 0; i < l; i++)
	{
		// Daca caracterul curent este un operand, atunci il vom adauga la output
		if (isalpha(infix[i]) || isdigit(infix[i]))
			output += infix[i];
		// Daca caracterul curent este '(', atunci il vom adauga la stiva
		else if (infix[i] == '(')
			push(s, '(');
		// Daca caracterul curent este ')', atunci vom sterge din stiva pana cand dam de '('
		else if (infix[i] == ')')
		{
			while (top(s) != '(')
			{
				output += top(s);
				pop(s);
			}
			// Eliminam '(' din stiva
			pop(s);
		}
		// Operator gasit
		else
		{
			if (esteOperator(top(s)))
			{
				if (infix[i] == '^')
				{
					while (prioritate(infix[i]) <= prioritate(top(s)))
					{
						output += top(s);
						pop(s);
					}
				}
				else
				{
					while (prioritate(infix[i]) < prioritate(top(s)))
					{
						output += top(s);
						pop(s);
					}
				}
				// Adaugam caracterul curent in stiva
				push(s, infix[i]);
			}
		}
	}
	while (s.varf != NULL)
	{
		output += top(s);
		pop(s);
	}

	return output;
}

void adaugareEntitateLaInceput(matriceDeCaractere& T, const entitate entitate)
{
	int i;
	for (i = T.dimensiuneTablouEntitati - 1; i >= 0; i--)
		strcpy(T.tablouEntitati[i + 1], T.tablouEntitati[i]);
	strcpy(T.tablouEntitati[0], entitate);
	T.dimensiuneTablouEntitati++;
}

void adaugareEntitateLaSfarsit(matriceDeCaractere& T, const entitate entitate)
{
	T.dimensiuneTablouEntitati++;
	strcpy(T.tablouEntitati[T.dimensiuneTablouEntitati - 1], entitate);
}

void infixCatrePostfix(matriceDeCaractere infix, matriceDeCaractere& postfix)
{
	matriceDeCaractere infixCopie; // Fiind matrice de caractere, modificarile asupra infix-ului raman dupa apelul functiei
	infixCopie.dimensiuneTablouEntitati = infix.dimensiuneTablouEntitati;
	for (int i = 0; i < infix.dimensiuneTablouEntitati; i++)
		strcpy(infixCopie.tablouEntitati[i], infix.tablouEntitati[i]);
	adaugareEntitateLaInceput(infixCopie, _parantezaRotundaDeschisa);
	adaugareEntitateLaSfarsit(infixCopie, _parantezaRotundaInchisa);
	int l = infixCopie.dimensiuneTablouEntitati;
	stivaE s;
	intializareStiva(s);
	postfix.dimensiuneTablouEntitati = 0;

	for (int i = 0; i < l; i++)
	{
		// Daca entitatea curenta este un operand, atunci il vom adauga la output (postfix)
		if (!esteOperator(infixCopie.tablouEntitati[i]) && !esteParanteza(infixCopie.tablouEntitati[i]))
		{
			adaugareEntitateLaSfarsit(postfix, infixCopie.tablouEntitati[i]);
		}
		// Daca entitatea curenta este "(", atunci o vom adauga la stiva
		else if (esteParantezaRotundaDeschisa(infixCopie.tablouEntitati[i]))
			push(s, _parantezaRotundaDeschisa);
		// Daca caracterul curent este ")", atunci vom sterge din stiva pana cand dam de "("
		else if (esteParantezaRotundaInchisa(infixCopie.tablouEntitati[i]))
		{
			while (!esteParantezaRotundaDeschisa(*top(s)))
			{
				adaugareEntitateLaSfarsit(postfix, *top(s));
				pop(s);
			}
			// Eliminam "(" din stiva
			pop(s);
		}
		// Operator gasit
		else
		{
			if (esteOperator(*top(s)) || esteParanteza(*top(s)))
			{

				if (esteOperatorControl(infixCopie.tablouEntitati[i]))
				{
					while (prioritate(infixCopie.tablouEntitati[i]) <= prioritate(*top(s)))
					{
						adaugareEntitateLaSfarsit(postfix, *top(s));
						pop(s);
					}
				}
				else
				{
					while (prioritate(infixCopie.tablouEntitati[i]) < prioritate(*top(s)))
					{
						adaugareEntitateLaSfarsit(postfix, *top(s));
						pop(s);
					}
				}
				// Adaugam caracterul curent in stiva
				push(s, infixCopie.tablouEntitati[i]);
			}
		}
	}
	while (s.varf != NULL)
	{
		adaugareEntitateLaSfarsit(postfix, *top(s));
		pop(s);
	}
}

string infixCatrePrefix(string infix)
{
	/**
	 Pasii determinarii notatiei prefixate
	 1. Inversam String
	 2. Inlocuim '(' cu ')' si vice versa
	 3. Determinam Postfix
	 4. Inversam postfix
	*/
	int l = infix.size();
	/// Inversam infix
	reverse(infix.begin(), infix.end());
	/// Inlocuim '(' cu ')' si vice versa
	for (int i = 0; i < l; i++)
	{
		if (infix[i] == '(')
			infix[i] = ')';
		else if (infix[i] == ')')
			infix[i] = '(';
	}
	/// Determinam Postfix
	string prefix = infixCatrePostfix(infix);
	/// Inversam postfix
	reverse(prefix.begin(), prefix.end());
	return prefix;
}

void infixCatrePrefix(matriceDeCaractere infix, matriceDeCaractere& prefix)
{
	/**
	 Pasii determinarii notatiei prefixate
	 1. Inversam infix
	 2. Inlocuim "(" cu ")" si vice versa
	 3. Determinam postfix
	 4. Inversam postfix
	*/

	matriceDeCaractere infixCopie; // Fiind matrice de caractere, modificarile asupra infix-ului raman dupa apelul functiei
	infixCopie.dimensiuneTablouEntitati = infix.dimensiuneTablouEntitati;
	for (int i = 0; i < infix.dimensiuneTablouEntitati; i++)
		strcpy(infixCopie.tablouEntitati[i], infix.tablouEntitati[i]);

	int l = infixCopie.dimensiuneTablouEntitati;
	/// Inversam infixCopie
	int st = 0, dr = l - 1;
	while (st < dr)
	{
		entitate aux;
		strcpy(aux, infixCopie.tablouEntitati[st]);
		strcpy(infixCopie.tablouEntitati[st], infixCopie.tablouEntitati[dr]);
		strcpy(infixCopie.tablouEntitati[dr], aux);
		st++;
		dr--;
	}
	/// Inlocuim "(" cu ")" si vice versa
	for (int i = 0; i < l; i++)
	{
		if (esteParantezaRotundaDeschisa(infixCopie.tablouEntitati[i]))
			strcpy(infixCopie.tablouEntitati[i], _parantezaRotundaInchisa);
		else if (esteParantezaRotundaInchisa(infixCopie.tablouEntitati[i]))
			strcpy(infixCopie.tablouEntitati[i], _parantezaRotundaDeschisa);
	}
	/// Determinam postfix
	prefix.dimensiuneTablouEntitati = 0;
	infixCatrePostfix(infixCopie, prefix);
	l = prefix.dimensiuneTablouEntitati; // Infix-ul si prefix-ul au dimensiuni diferite
	/// Inversam postfix
	st = 0, dr = l - 1;
	while (st < dr)
	{
		entitate aux;
		strcpy(aux, prefix.tablouEntitati[st]);
		strcpy(prefix.tablouEntitati[st], prefix.tablouEntitati[dr]);
		strcpy(prefix.tablouEntitati[dr], aux);
		st++;
		dr--;
	}
}

// Conversie de la prefixat la vector de pozitii (functioneaza doar in cazul in care operanzii sunt caractere, nu entitati - in primele versiuni ale proiectului)

int prefixCatreArboreTablou(int poz, int idx, int& dimensiuneTablouri, string s, char arboreTablou[]) /// idx in arbore
{
	// Nu mai am nevoie pentru matricea de caractere
	if (s[poz] == '*' || s[poz] == '+' || s[poz] == '-' || s[poz] == '/' || s[poz] == '^')
	{
		arboreTablou[idx] = s[poz];
		int newpoz = prefixCatreArboreTablou(poz + 1, idx * 2, dimensiuneTablouri, s, arboreTablou);
		int newpoz2 = prefixCatreArboreTablou(newpoz, idx * 2 + 1, dimensiuneTablouri, s, arboreTablou);
		return newpoz2;
	}
	else
	{
		arboreTablou[idx] = s[poz];
		dimensiuneTablouri = max(dimensiuneTablouri, idx);
		return poz + 1;
	}
}

void afiseazaPozitiaInArboreSiCaracterulDinNod(char arboreTablou[], int dimensiuneTablouri) // Nu mai am nevoie pentru matricea de caractere
{
	cout << "(Pentru expresii cu operanzi de lungime 1) Vectorul de pozitii este:\n";

	for (int i = 1; i <= dimensiuneTablouri; i++)
		cout << i << " ";

	cout << '\n';

	if (dimensiuneTablouri >= 100)
	{
		for (int i = 1; i <= 9; i++)
			cout << arboreTablou[i] << " ";

		for (int i = 10; i <= 99; i++)
			cout << arboreTablou[i] << "  ";

		for (int i = 100; i <= dimensiuneTablouri; i++)
			cout << arboreTablou[i] << "  ";
	}
	else if (dimensiuneTablouri >= 10 && dimensiuneTablouri <= 99)
	{
		for (int i = 1; i <= 9; i++)
			cout << arboreTablou[i] << " ";

		for (int i = 10; i <= dimensiuneTablouri; i++)
			cout << arboreTablou[i] << "  ";
	}
	else if (dimensiuneTablouri <= 9)
	{
		for (int i = 1; i <= 9; i++)
			cout << arboreTablou[i] << " ";
	}
	cout << '\n';
}

void umplereCuSemnulIntrebarii(char arboreTablou[], int dimensiuneTablouri) // Nu mai am nevoie pentru matricea de caractere (in versiunile mai noi ale proiectului, cu matrice de caractere (entitati), am construit arborele expresiei direct din prefix)
{
	for (int i = 1; i <= dimensiuneTablouri; i++)
		if (arboreTablou[i] == 0)
			arboreTablou[i] = '?';
}

// -----------------------
// Construirea vectorului de tati pentru arbore (pentru versiunile initiale ale proiectului)
// -----------------------

void construireVectorDeTati(char poz[], int t[], int dimensiuneTablouri) // nu mai am nevoie pentru matricea de caractere
{
	t[1] = 0;
	for (int i = 1; i <= dimensiuneTablouri - 2; i++)
		if (poz[i] != '?')
		{
			t[i * 2] = i;
			t[i * 2 + 1] = i;
		}
		else
			t[i] = -1;
}

// -----------------------
// Construirea vectorilor fiu_st[] si fiu_dr[] pentru arbore (pentru versiunile initiale ale proiectului)
// -----------------------

void construireVectorFiuStSiFiuDr(int t[], int fiu_st[], int fiu_dr[], int dimensiuneTablouri) // Nu mai am nevoie pentru matricea de caractere
{
	for (int i = 1; i <= dimensiuneTablouri; i++)
	{
		fiu_dr[i] = 0;
		fiu_st[i] = 0;
	}
	for (int i = 2; i <= dimensiuneTablouri; i++)
		if (t[i] != -1)
		{
			if (i % 2 == 0)
				fiu_st[t[i]] = i;
			else
				fiu_dr[t[i]] = i;
		}
}

// -----------------------
// Reprezentarea arborescenta a ecuatiei: construire arbori suport pentru ecuatie, reprezentare grafica arbori.
// Functiile din primele iteratii ale proiectului care operau doar cu expresii
// in care operanzii aveau lungimea 1 (litere sau cifre), au fost rescrise (in iteratiile mai noi), prin supraincarcare,
// cu functii care opereazacu entitati (operanzi de lungime oarecare).
// Functiile vechi au fost pastrate pentru a verifica daca noile functii
// opereaza corect (si cele vechi, si cele noi trebuie sa dea aceleasi rezultate pentru expresii cu operanzi de lungime 1).
// -----------------------

void adaugareNod(arbore& a, char k)
{
	a = new nodA;
	a->info = k;
	a->st = NULL;
	a->dr = NULL;
}

void adaugareNod(arboreE& a, const entitate k)
{
	a = new nodAE;
	strcpy(a->info, k);
	a->st = NULL;
	a->dr = NULL;
}

int radacina(int T[], int dimensiuneTablouri) // Nu mai am nevoie pentru matricea de caractere (pentru versiunile initiale ale proiectului)
{
	int i;
	for (i = 1; i <= dimensiuneTablouri; i++)
		if (T[i] == 0)
			return i;
	return 0;
}

void construireArbore(arbore& a, int k, int fiu_st[], int fiu_dr[], char arboreTablou[])
{
	if (k != 0) // Cheia nodului este si numarul sau de ordine (un numar de la 1 la nrNoduri)
	{
		adaugareNod(a, arboreTablou[k]);
		construireArbore(a->st, fiu_st[k], fiu_st, fiu_dr, arboreTablou);
		construireArbore(a->dr, fiu_dr[k], fiu_st, fiu_dr, arboreTablou);
	}
}

int construireArbore(arboreE& a, matriceDeCaractere prefix, int idx)
{

	adaugareNod(a, prefix.tablouEntitati[idx]);
	if (esteOperator(prefix.tablouEntitati[idx]))
	{
		idx = construireArbore(a->st, prefix, idx + 1);
		idx = construireArbore(a->dr, prefix, idx + 1);
	}
	else if (esteFunctie(prefix.tablouEntitati[idx]))
		idx = construireArbore(a->st, prefix, idx + 1);
	return idx;
}

int numarLinii(arbore a)
{
	if (a == NULL)
		return 0;
	else
	{
		int nrl_st = numarLinii(a->st);
		int nrl_dr = numarLinii(a->dr);
		return max(nrl_st, nrl_dr) + 1;
	}
}

int numarLinii(arboreE a)
{
	if (a == NULL)
		return 0;
	else
	{
		int nrl_st = numarLinii(a->st);
		int nrl_dr = numarLinii(a->dr);
		return max(nrl_st, nrl_dr) + 1;
	}
}

int numarColoane(arbore a)
{
	if (a == NULL)
		return 0;
	else
	{
		int nrc_st = numarColoane(a->st);
		int nrc_dr = numarColoane(a->dr);
		return nrc_st + 1 + nrc_dr;
	}
}

int numarColoane(arboreE a)
{
	if (a == NULL)
		return 0;
	else
	{
		int nrc_st = numarColoane(a->st);
		int nrc_dr = numarColoane(a->dr);
		return nrc_st + 1 + nrc_dr;
	}
}

void repartizareLinieSiColoanaNoduriArbore(arbore& a, int linie, int coloana)
{
	a->linie = linie;
	a->coloana = coloana;
	arbore fiuStang = a->st;
	if (fiuStang != NULL)
		repartizareLinieSiColoanaNoduriArbore(fiuStang, linie + 1, coloana - numarColoane(fiuStang->dr) - 1);
	arbore fiuDrept = a->dr;
	if (fiuDrept != NULL)
		repartizareLinieSiColoanaNoduriArbore(fiuDrept, linie + 1, coloana + numarColoane(fiuDrept->st) + 1);
}

void repartizareLinieSiColoanaNoduriArbore(arboreE& a, int linie, int coloana)
{
	a->linie = linie;
	a->coloana = coloana;
	arboreE fiuStang = a->st;
	if (fiuStang != NULL)
		repartizareLinieSiColoanaNoduriArbore(fiuStang, linie + 1, coloana - numarColoane(fiuStang->dr) - 1);
	arboreE fiuDrept = a->dr;
	if (fiuDrept != NULL)
		repartizareLinieSiColoanaNoduriArbore(fiuDrept, linie + 1, coloana + numarColoane(fiuDrept->st) + 1);
}

void afisareArbore(arbore nodCurent, arbore nodParinte, int lungimeCelula, int inaltimeCelula, int razaCerc)
{
	if (nodCurent != NULL)
	{
		ostringstream str1;
		str1 << nodCurent->info;
		string str2 = str1.str();
		char* str = &str2[0];
		int x = (nodCurent->coloana - 1) * lungimeCelula + lungimeCelula / 2;
		int y = (nodCurent->linie - 1) * inaltimeCelula + inaltimeCelula / 2;
		setcolor(1);
		circle(x, y, razaCerc); // Desenez nodul curent
		settextstyle(8, 0, 3);
		setcolor(3);
		outtextxy(x - 10, y - 7, str);
		if (nodParinte != NULL) // Desenez muchia catre parinte daca nu sunt in radacina (daca am ascendent)
		{
			int x_Parinte = (nodParinte->coloana - 1) * lungimeCelula + lungimeCelula / 2;
			int y_Parinte = (nodParinte->linie - 1) * inaltimeCelula + inaltimeCelula / 2;
			setcolor(4);
			line(x, y, x_Parinte, y_Parinte);
		}
		afisareArbore(nodCurent->st, nodCurent, lungimeCelula, inaltimeCelula, razaCerc);
		afisareArbore(nodCurent->dr, nodCurent, lungimeCelula, inaltimeCelula, razaCerc);
	}
}

void afisareArbore(arboreE nodCurent, arboreE nodParinte, int lungimeCelula, int inaltimeCelula, int razaCerc)
{
	if (nodCurent != NULL)
	{
		//https://home.cs.colorado.edu/~main/bgi/doc/setcolor.html
		int x = (nodCurent->coloana - 1) * lungimeCelula + lungimeCelula / 2;
		int y = (nodCurent->linie - 1) * inaltimeCelula + inaltimeCelula / 2;
		//setcolor(9);
		circle(x, y, razaCerc); // Desenez nodul curent

		if (nodParinte != NULL) // Desenez muchia catre parinte daca nu sunt radacina
		{
			int x_Parinte = (nodParinte->coloana - 1) * lungimeCelula + lungimeCelula / 2;
			int y_Parinte = (nodParinte->linie - 1) * inaltimeCelula + inaltimeCelula / 2;
			//setcolor(12);
			line(x, y, x_Parinte, y_Parinte);
		}

		afisareArbore(nodCurent->st, nodCurent, lungimeCelula, inaltimeCelula, razaCerc);
		afisareArbore(nodCurent->dr, nodCurent, lungimeCelula, inaltimeCelula, razaCerc);

		settextstyle(8, 0, 3);
		setcolor(COLOR(71, 255, 10));
		setbkcolor(BLACK);
		outtextxy(x - textwidth(nodCurent->info) / 2, y - textheight(nodCurent->info) / 2, nodCurent->info);
		setcolor(Culori[index_culoareArbore].culoare);
	}
}

// -----------------------
// Afisari informatii la consola pentru verificarea calculelor (verificarea functiilor scrise)
// -----------------------

void afisareLiniute()
{
	cout << "-----------------------";
	cout << '\n';
}

void afisareTablouEntitati(matriceDeCaractere T) // T poate fi infix-ul, postfix-ul sau prefix-ul de entitati (pentru expresiile cu operanzi de orice marime)
{
	int i;
	cout << "Lista entitatilor din ecuatie este:\n";
	for (i = 0; i < T.dimensiuneTablouEntitati; i++)
		cout << T.tablouEntitati[i] << '\n';
	afisareLiniute();
}

void afisareVectoriDeTati(int T[], int dimensiuneTablouri) // Nu mai am nevoie pentru matricea de caractere (doar pentru versiunile initiale ale proiectului cand operanzii erau caractere)
{
	// Verificare vectori de tati
	cout << "(Pentru expresii cu operanzi de lungime 1) Vectorul de tati: " << '\n';
	for (int i = 1; i <= dimensiuneTablouri; i++)
		cout << T[i] << ' ';
	cout << '\n';
	afisareLiniute();
}

void afisareVectorDeFiiStangi(int fiu_st[], int dimensiuneTablouri) // Nu mai am nevoie pentru matricea de caractere
{
	// Verificare vector fii stangi
	cout << "(Pentru expresii cu operanzi de lungime 1) Vectorul fiu_st: " << '\n';
	for (int i = 1; i <= dimensiuneTablouri; i++)
		cout << fiu_st[i] << ' ';
	cout << '\n';
	afisareLiniute();
}

void afisareVectorDeFiiDrepti(int fiu_dr[], int dimensiuneTablouri) // Nu mai am nevoie pentru matricea de caractere
{
	// Verificare vector fii drepti
	cout << "(Pentru expresii cu operanzi de lungime 1) Vectorul fiu_dr: " << '\n';
	for (int i = 1; i <= dimensiuneTablouri; i++)
		cout << fiu_dr[i] << ' ';
	cout << '\n';
	afisareLiniute();
}

void afisareInfixCreareSiAfisarePostfixPrefix(string infix, string& postfix, string& prefix)
{
	cout << "(Pentru expresii cu operanzi de lungime 1) Infix este: " << infix << '\n'; // a+(b*c))-d
	afisareLiniute();

	postfix = infixCatrePostfix(infix); // abc*+d-
	cout << "(Pentru expresii cu operanzi de lungime 1) Postfix este: " << postfix << '\n';
	afisareLiniute();

	prefix = infixCatrePrefix(infix);
	cout << "(Pentru expresii cu operanzi de lungime 1) Prefix este: " << prefix << '\n'; // -+a*bcd
	afisareLiniute();
}

void afisareInfixCreareSiAfisarePostfixPrefix(matriceDeCaractere infix, matriceDeCaractere& postfix, matriceDeCaractere& prefix)
{
	int i;

	cout << "_Infix este: ";
	for (i = 0; i < infix.dimensiuneTablouEntitati; i++)
		cout << infix.tablouEntitati[i] << ' ';
	cout << '\n'; // a+(b*c))-d
	afisareLiniute();

	infixCatrePostfix(infix, postfix); // abc*+d-
	cout << "_Postfix este: ";
	for (i = 0; i < postfix.dimensiuneTablouEntitati; i++)
		cout << postfix.tablouEntitati[i] << ' ';
	cout << '\n';
	afisareLiniute();

	infixCatrePrefix(infix, prefix);
	cout << "_Prefix este: ";
	for (i = 0; i < prefix.dimensiuneTablouEntitati; i++)
		cout << prefix.tablouEntitati[i] << ' ';
	cout << '\n'; // -+a*bcd
	afisareLiniute();
}

// -----------------------
// Actualizari (sau generari insotite de actualizari) arbori (suport pentru Prefix) in vederea pregatirii reprezentarii
// grafice a acestora
// -----------------------

void construireSiActualizareArbore(arbore& a, int T[], int fiu_st[], int fiu_dr[], int lungimePlansa, int inaltimePlansa,
	int& lungimeCelula, int& inaltimeCelula, int& razaCerc, char arboreTablou[], int dimensiuneTablouri)
{
	int nrL, nrC, coloanaRadacina, R;
	R = radacina(T, dimensiuneTablouri);
	construireArbore(a, R, fiu_st, fiu_dr, arboreTablou);
	nrL = numarLinii(a);
	nrC = numarColoane(a);
	lungimeCelula = lungimePlansa / nrC;
	inaltimeCelula = inaltimePlansa / nrL;
	razaCerc = min(lungimeCelula, inaltimeCelula) / 2;
	coloanaRadacina = numarColoane(a->st) + 1;
	repartizareLinieSiColoanaNoduriArbore(a, 1, coloanaRadacina);
}

void SRD(arboreE a) // Am nevoie sa verific/afisez informatiile din nodurile arborelui
{
	if (a)
	{
		SRD(a->st);
		cout << a->info << " ";
		SRD(a->dr);
	}
}

void construireSiActualizareArbore(arboreE& a, int lungimePlansa, int inaltimePlansa,
	int& lungimeCelula, int& inaltimeCelula, int& razaCerc, matriceDeCaractere prefix)
{
	int nrL, nrC, coloanaRadacina;

	construireArbore(a, prefix, 0);
	//SRD(a);

	nrL = numarLinii(a);
	nrC = numarColoane(a);
	lungimeCelula = lungimePlansa / nrC;
	inaltimeCelula = inaltimePlansa / nrL;
	razaCerc = min(lungimeCelula, inaltimeCelula) / 2;
	coloanaRadacina = numarColoane(a->st) + 1;
	repartizareLinieSiColoanaNoduriArbore(a, 1, coloanaRadacina);
}

void actualizareArboreCuParanteze(arbore a, int lungimePlansa, int inaltimePlansa, int& lungimeCelula, int& inaltimeCelula, int& razaCerc)
{
	int nrL, nrC, coloanaRadacina;
	nrL = numarLinii(a);
	nrC = numarColoane(a);
	lungimeCelula = lungimePlansa / nrC;
	inaltimeCelula = inaltimePlansa / nrL;
	razaCerc = min(lungimeCelula, inaltimeCelula) / 2;
	coloanaRadacina = numarColoane(a->st) + 1;
	repartizareLinieSiColoanaNoduriArbore(a, 1, coloanaRadacina);
}

void actualizareArboreCuParanteze(arboreE a, int lungimePlansa, int inaltimePlansa, int& lungimeCelula, int& inaltimeCelula, int& razaCerc)
{
	int nrL, nrC, coloanaRadacina;
	nrL = numarLinii(a);
	nrC = numarColoane(a);
	lungimeCelula = lungimePlansa / nrC;
	inaltimeCelula = inaltimePlansa / nrL;
	razaCerc = min(lungimeCelula, inaltimeCelula) / 2;
	coloanaRadacina = numarColoane(a->st) + 1;
	repartizareLinieSiColoanaNoduriArbore(a, 1, coloanaRadacina);
}

// -----------------------
// Determinarea lungimii arborelui suport al ecuatiei
// -----------------------

int lungimeEcuatie(arbore a)
{
	int nr_st, nr_dr;
	if (a)
	{
		nr_st = lungimeEcuatie(a->st);
		nr_dr = lungimeEcuatie(a->dr);
		if (a->info == '/')
			return max(nr_st, nr_dr);
		else
			return nr_st + 1 + nr_dr;
	}
	else
		return 0;
}

int lungimeEcuatie(arboreE a)
{
	int nr_st, nr_dr;
	if (a)
	{
		nr_st = lungimeEcuatie(a->st);
		nr_dr = lungimeEcuatie(a->dr);
		if (esteOperatorSlash(a->info))
			return max(nr_st, nr_dr);
		else
			if (esteFunctie(a->info))
				return nr_st + (strlen(a->info) + 2) + nr_dr; // Tinem cont si de parantezele care se vor adauga la numele functiei
			else
				return nr_st + strlen(a->info) + nr_dr;
	}
	else
		return 0;
}

void afisareLungimeEcuatie(int val)
{
	cout << "Numarul de coloane pe care este afisata ecuatia (in arborele cu paranteze adaugate): " << val << '\n';
	afisareLiniute();
}

// -----------------------
// Determinarea inaltimii arborelui suport al ecuatiei
// -----------------------

int inaltimeEcuatie(arbore a)
{
	if (a)
	{
		int nr_st = inaltimeEcuatie(a->st);
		int nr_dr = inaltimeEcuatie(a->dr);
		if (a->info == '/')
			return nr_st + 1 + nr_dr;
		else
			return max(max(nr_st, nr_dr), 1);
	}
	else
		return 0;
}

int inaltimeEcuatie(arboreE a)
{
	if (a)
	{
		int nr_st = inaltimeEcuatie(a->st);
		int nr_dr = inaltimeEcuatie(a->dr);
		if (esteOperatorSlash(a->info))
			return nr_st + 1 + nr_dr;
		else
			return max(max(nr_st, nr_dr), 1);
	}
	else
		return 0;
}

void afisareInaltimeEcuatie(int val)
{
	cout << "Numarul de linii pe care este afisata ecuatia (in arborele cu paranteze adaugate): " << val << '\n';
	afisareLiniute();
}

// -----------------------
// Adaugare paranteze in arborele ecuatiei
// -----------------------

void duplicareArbore(arbore a, arbore& b)
{
	if (a)
	{
		b = new nodA;
		*b = *a;
		duplicareArbore(a->st, b->st);
		duplicareArbore(a->dr, b->dr);
	}
	else
		b = NULL;
}

void duplicareArbore(arboreE a, arboreE& b)
{
	if (a)
	{
		b = new nodAE;
		*b = *a;
		duplicareArbore(a->st, b->st);
		duplicareArbore(a->dr, b->dr);
	}
	else
		b = NULL;
}

void adaugareParantezaSt(arbore& a)
{
	nodA* aux = a;
	while (aux->st != NULL)
		aux = aux->st;
	adaugareNod(aux->st, '(');
}

void adaugareParantezaSt(arboreE& a)
{
	nodAE* aux = a;
	while (aux->st != NULL)
		aux = aux->st;
	adaugareNod(aux->st, _parantezaRotundaDeschisa);
}

void adaugareParantezaDr(arbore& a)
{
	nodA* aux = a;
	while (aux->dr != NULL)
		aux = aux->dr;
	adaugareNod(aux->dr, ')');
}

void adaugareParantezaDr(arboreE& a)
{
	nodAE* aux = a;
	while (aux->dr != NULL)
		aux = aux->dr;
	adaugareNod(aux->dr, _parantezaRotundaInchisa);
}

void adaugareParanteze(arbore& a)
{
	if (a)
	{
		adaugareParanteze(a->st);

		if (a->info == '+' || a->info == '-')
		{
			adaugareParantezaSt(a);
			adaugareParantezaDr(a);
		}

		adaugareParanteze(a->dr);
	}
}

void adaugareParanteze(arboreE& a)
{
	if (a)
	{
		adaugareParanteze(a->st);

		if (esteOperatorPlus(a->info) || esteOperatorMinus(a->info))
		{
			adaugareParantezaSt(a);
			adaugareParantezaDr(a);
		}

		adaugareParanteze(a->dr);
	}
}

/*
In iteratiile de ordin 3 am adaugat paranteze numai acolo unde erau necesare, drept urmare am rescris functia adaugareParanteze()
precum si functiile adaugareParantezaSt() si adaugareParantezaDr() care se vor apela dupa actualizarea liniilor
entitatilor din arborele expresiei si inaintea actualizarii coloanelor entitatilor din arborele expresiei
*/

void adaugareParantezaSt(arboreE& a, int linieOperator)
{
	nodAE* aux = a;
	while (aux->st != NULL)
		aux = aux->st;
	adaugareNod(aux->st, _parantezaRotundaDeschisa);
	aux->st->linie = linieOperator;
}

void adaugareParantezaDr(arboreE& a, int linieOperator)
{
	nodAE* aux = a;
	while (aux->dr != NULL)
		aux = aux->dr;
	adaugareNod(aux->dr, _parantezaRotundaInchisa);
	aux->dr->linie = linieOperator;
}

void adaugareParanteze(arboreE& a, arboreE nodParinte)
{
	if (a)
	{
		adaugareParanteze(a->st, a);

		if (nodParinte && (esteOperatorAsterisc(nodParinte->info) || esteOperatorControl(nodParinte->info)) && (esteOperatorPlus(a->info) || esteOperatorMinus(a->info)))
		{
			adaugareParantezaSt(a, a->linie);
			adaugareParantezaDr(a, a->linie);
		}

		adaugareParanteze(a->dr, a);
	}
}

void adaugareFunctieSt(arboreE& a, entitate functie, int linieOperator)
{
	nodAE* aux = a;
	while (aux->st != NULL)
		aux = aux->st;
	adaugareNod(aux->st, functie);
	aux->st->linie = linieOperator;
}

void actualizareArboreExpresieCuFunctii(arboreE& a, arboreE nodParinte, arboreE nodParinteParinte)
{
	if (a)
	{
		actualizareArboreExpresieCuFunctii(a->st, a, nodParinte);
		actualizareArboreExpresieCuFunctii(a->dr, a, nodParinte);

		if (nodParinte && esteFunctie(nodParinte->info))
		{
			adaugareParantezaSt(a, a->linie);
			// Adaug functia, ca frunza, in stanga
			adaugareFunctieSt(a, nodParinte->info, a->linie);
			// Elimin nodul curent corespunzator functiei
			if (nodParinteParinte) // Functia nu este in radacina arborelui
			{
				if (nodParinteParinte->st && esteFunctie(nodParinteParinte->st->info) && nodParinteParinte->st->st == a)
					nodParinteParinte->st = a;
				if (nodParinteParinte->st && esteFunctie(nodParinteParinte->st->info) && nodParinteParinte->st->dr == a)
					nodParinteParinte->st = a;
				if (nodParinteParinte->dr && esteFunctie(nodParinteParinte->dr->info) && nodParinteParinte->dr->st == a)
					nodParinteParinte->dr = a;
				if (nodParinteParinte->dr && esteFunctie(nodParinteParinte->dr->info) && nodParinteParinte->dr->dr == a)
					nodParinteParinte->dr = a;
				//delete nodParinte;
			}
			else // Functia este in radacina arborelui
			{

			}
			adaugareParantezaDr(a, a->linie);
		}

	}
}

void actualizareArboreExpresieCuFuctiiInRadacina(arboreE& a)
{
	// Daca unul dintre nodurile arborelui contine o functie atunci el are doar fiu stang
	while (esteFunctie(a->info))
	{
		arboreE aux = a;
		a = a->st;
		delete aux;
	}
}

// -----------------------
// Actualizarea nodurilor arborelui suport pentru reprezentarea grafica a ecuatiei cu informatii legate
// de locul unde vor fi plasate pe plansa cheile arborelui (operatori, operanzi, paranteze)
// -----------------------

void intitializareLiniiColoaneCuInfinit(arbore a)
{
	if (a)
	{
		a->linie = a->coloana = INFINIT;

		a->numarColoaneLinieFractie = 0;

		intitializareLiniiColoaneCuInfinit(a->st);

		intitializareLiniiColoaneCuInfinit(a->dr);
	}
}

void intitializareLiniiColoaneCuInfinit(arboreE a)
{
	if (a)
	{
		a->linie = a->coloana = INFINIT;

		a->numarColoaneLinieFractie = 0;

		intitializareLiniiColoaneCuInfinit(a->st);

		intitializareLiniiColoaneCuInfinit(a->dr);
	}
}

void repartizareNoduriArboreExpresiePeLiniiFractie(arbore& a, int linieAnterioaraFractie, int& linieUrmatoareFractie,
	int deUndeVin, bool amPredecesorFractie)
{
	if (a != NULL)
	{
		int linieCurenta = linieAnterioaraFractie, linieUrmatoareaFractieSt, linieUrmatoareaFractieDr;
		if (a->info == '/')
		{
			if (amPredecesorFractie)
				if (deUndeVin <= 0)
					linieCurenta += 2;
				else
					linieCurenta -= 2;
			a->linie = linieCurenta;
			linieUrmatoareFractie = linieCurenta;
			amPredecesorFractie = true;
		}
		else
			linieUrmatoareFractie = INFINIT;
		if (a->info == '/')
		{
			repartizareNoduriArboreExpresiePeLiniiFractie(a->st, linieCurenta, linieUrmatoareaFractieSt, 1, amPredecesorFractie);
			repartizareNoduriArboreExpresiePeLiniiFractie(a->dr, linieCurenta, linieUrmatoareaFractieDr, -1, amPredecesorFractie);
		}
		else
		{
			repartizareNoduriArboreExpresiePeLiniiFractie(a->st, linieCurenta, linieUrmatoareaFractieSt, deUndeVin, amPredecesorFractie);
			repartizareNoduriArboreExpresiePeLiniiFractie(a->dr, linieCurenta, linieUrmatoareaFractieDr, deUndeVin, amPredecesorFractie);
		}
		if (a->info != '/')
			a->linie = min(linieUrmatoareaFractieSt, linieUrmatoareaFractieDr);
	}
	else
		linieUrmatoareFractie = INFINIT;
}

int inaltimeEcuatieLaNumitori(arboreE a)
{
	if (a)
	{
		int nr_st = inaltimeEcuatieLaNumitori(a->st);
		int nr_dr = inaltimeEcuatieLaNumitori(a->dr);
		if (esteOperatorSlash(a->info))
			return inaltimeEcuatie(a->dr);
		else
			return max(nr_st, nr_dr);
	}
	else
		return 0;
}

int inaltimeEcuatieLaNumaratori(arboreE a)
{
	if (a)
	{
		int nr_st = inaltimeEcuatieLaNumaratori(a->st);
		int nr_dr = inaltimeEcuatieLaNumaratori(a->dr);
		if (esteOperatorSlash(a->info))
			return inaltimeEcuatie(a->st);
		else
			return max(nr_st, nr_dr);
	}
	else
		return 0;
}

// Abandonata in iteratiile de ordin 3
void repartizareNoduriArboreExpresiePeLiniiFractie(arboreE& a, int linieAnterioaraFractie, int& linieUrmatoareFractie,
	int deUndeVin, bool amPredecesorFractie)
{
	if (a != NULL)
	{
		int linieCurenta = linieAnterioaraFractie, linieUrmatoareaFractieSt, linieUrmatoareaFractieDr;
		if (esteOperatorSlash(a->info))
		{
			if (amPredecesorFractie)
				if (deUndeVin <= 0)
				{
					//linieCurenta += 2;
					linieCurenta += inaltimeEcuatieLaNumaratori(a) + 1;
				}
				else
				{
					//linieCurenta -= 2;
					linieCurenta -= inaltimeEcuatieLaNumitori(a) + 1;
				}
			a->linie = linieCurenta;
			linieUrmatoareFractie = linieCurenta;
			amPredecesorFractie = true;
		}
		else
			linieUrmatoareFractie = INFINIT;
		if (esteOperatorSlash(a->info))
		{
			repartizareNoduriArboreExpresiePeLiniiFractie(a->st, linieCurenta, linieUrmatoareaFractieSt, 1, amPredecesorFractie);
			repartizareNoduriArboreExpresiePeLiniiFractie(a->dr, linieCurenta, linieUrmatoareaFractieDr, -1, amPredecesorFractie);
		}
		else
		{
			repartizareNoduriArboreExpresiePeLiniiFractie(a->st, linieCurenta, linieUrmatoareaFractieSt, deUndeVin, amPredecesorFractie);
			repartizareNoduriArboreExpresiePeLiniiFractie(a->dr, linieCurenta, linieUrmatoareaFractieDr, deUndeVin, amPredecesorFractie);
		}
		if (!esteOperatorSlash(a->info))
			a->linie = min(linieUrmatoareaFractieSt, linieUrmatoareaFractieDr);
	}
	else
		linieUrmatoareFractie = INFINIT;
}

void repartizareNoduriArboreExpresiePeLiniiNumaratorNumitor(arbore& a, int linieCurenta)
{
	if (a != NULL)
	{
		if (a->linie == INFINIT)
			a->linie = linieCurenta;
		if (a->info == '/')
		{
			repartizareNoduriArboreExpresiePeLiniiNumaratorNumitor(a->st, a->linie - 1);
			repartizareNoduriArboreExpresiePeLiniiNumaratorNumitor(a->dr, a->linie + 1);
		}
		else
		{
			repartizareNoduriArboreExpresiePeLiniiNumaratorNumitor(a->st, a->linie);
			repartizareNoduriArboreExpresiePeLiniiNumaratorNumitor(a->dr, a->linie);
		}
	}
}

// Abandonata in iteratiile de ordin 3
void repartizareNoduriArboreExpresiePeLiniiNumaratorNumitor(arboreE& a, int linieCurenta)
{
	if (a != NULL)
	{
		if (a->linie == INFINIT)
			a->linie = linieCurenta;
		if (esteOperatorSlash(a->info))
		{
			repartizareNoduriArboreExpresiePeLiniiNumaratorNumitor(a->st, a->linie - 1);
			//repartizareNoduriArboreExpresiePeLiniiNumaratorNumitor(a->st, a->linie - inaltimeEcuatieLaNumitori(a->st));
			repartizareNoduriArboreExpresiePeLiniiNumaratorNumitor(a->dr, a->linie + 1);
			//repartizareNoduriArboreExpresiePeLiniiNumaratorNumitor(a->st, a->linie + inaltimeEcuatieLaNumaratori(a->dr));
		}
		else
		{
			repartizareNoduriArboreExpresiePeLiniiNumaratorNumitor(a->st, a->linie);
			repartizareNoduriArboreExpresiePeLiniiNumaratorNumitor(a->dr, a->linie);
		}
	}
}

// Inlocuieste repartizareNoduriArboreExpresiePeLiniiFractie() si repartizareNoduriArboreExpresiePeLiniiNumaratorNumitor() in iteratiile de ordin 3 ale proiectului
void repartizareNoduriArboreExpresiePeLinii(arboreE& a, arboreE nodPredecesorFractie, int deUndeVin)
{
	if (a != NULL)
	{
		int linieCurenta;
		if (nodPredecesorFractie)
		{
			linieCurenta = nodPredecesorFractie->linie;
			if (deUndeVin < 0) // Vin din stanga
				linieCurenta += inaltimeEcuatieLaNumaratori(nodPredecesorFractie->dr) + 1;
			if (deUndeVin > 0) // Vin din dreapta
				linieCurenta -= inaltimeEcuatieLaNumitori(nodPredecesorFractie->st) + 1;
		}
		else
			linieCurenta = 0;

		a->linie = linieCurenta;

		if (esteOperatorSlash(a->info))
		{
			repartizareNoduriArboreExpresiePeLinii(a->st, a, 1);
			repartizareNoduriArboreExpresiePeLinii(a->dr, a, -1);
		}
		else
		{
			repartizareNoduriArboreExpresiePeLinii(a->st, nodPredecesorFractie, deUndeVin);
			repartizareNoduriArboreExpresiePeLinii(a->dr, nodPredecesorFractie, deUndeVin);
		}
	}
}

void repartizareNoduriArboreExpresiePeColoane(arbore& a, int coloanaCurenta)
{
	if (a != NULL)
	{
		int lungimeDr, lungimeSt, lungimeNumarator, lungimeNumitor;
		a->coloana = coloanaCurenta;
		if (a->info == '/')
		{
			repartizareNoduriArboreExpresiePeColoane(a->st, coloanaCurenta);
			repartizareNoduriArboreExpresiePeColoane(a->dr, coloanaCurenta);
		}
		else
		{
			if (a->st)
				if (a->st->info == '/')
				{
					lungimeNumarator = lungimeEcuatie(a->st->st);

					lungimeNumitor = lungimeEcuatie(a->st->dr);

					lungimeDr = max(lungimeNumarator, lungimeNumitor) / 2;
				}
				else
					lungimeDr = lungimeEcuatie(a->st->dr);
			else
				lungimeDr = 0;

			repartizareNoduriArboreExpresiePeColoane(a->st, coloanaCurenta - lungimeDr - 1);

			if (a->dr)
				if (a->dr->info == '/')
				{
					lungimeNumarator = lungimeEcuatie(a->dr->st);

					lungimeNumitor = lungimeEcuatie(a->dr->dr);

					lungimeSt = max(lungimeNumarator, lungimeNumitor) / 2;
				}
				else
					lungimeSt = lungimeEcuatie(a->dr->st);
			else
				lungimeSt = 0;
			repartizareNoduriArboreExpresiePeColoane(a->dr, coloanaCurenta + lungimeSt + 1);
		}
	}
}

/*
void repartizareNoduriArboreExpresiePeColoane(arboreE& a, float coloanaCurenta)
{
	if (a != NULL)
	{
		float lungimeDr, lungimeSt;
		int lungimeNumarator, lungimeNumitor;
		a->coloana = coloanaCurenta;
		if (esteOperator(a->info)) // a->st si a->dr sunt operatori sau operanzi
			if (esteOperatorSlash(a->info))
			{
				if (esteOperand(a->st->info) || esteOperatorSlash(a->st->info))
					repartizareNoduriArboreExpresiePeColoane(a->st, coloanaCurenta);
				else // +, -, *
				{
					lungimeDr = lungimeEcuatie(a->st->dr);
					repartizareNoduriArboreExpresiePeColoane(a->st, coloanaCurenta + (float)a->numarColoaneLinieFractie / 2 - lungimeDr - 0.5);
				}
				if (esteOperand(a->dr->info) || esteOperatorSlash(a->dr->info))
					repartizareNoduriArboreExpresiePeColoane(a->dr, coloanaCurenta);
				else // +, -, *
				{
					lungimeSt = lungimeEcuatie(a->dr->st);
					repartizareNoduriArboreExpresiePeColoane(a->dr, coloanaCurenta - (float)a->numarColoaneLinieFractie / 2 + lungimeSt + 0.5);
				}
			}
			else // a->info este +, -, *
			{
				if (esteOperatorSlash(a->st->info))
				{
					lungimeNumarator = lungimeEcuatie(a->st->st);

					lungimeNumitor = lungimeEcuatie(a->st->dr);

					lungimeDr = (float)max(lungimeNumarator, lungimeNumitor) / 2;

					repartizareNoduriArboreExpresiePeColoane(a->st, coloanaCurenta - lungimeDr - 0.5);
				}
				else // a->st->info este +, -, * sau operand
				{
					lungimeDr = lungimeEcuatie(a->st->dr);
					repartizareNoduriArboreExpresiePeColoane(a->st, coloanaCurenta - lungimeDr - (float)strlen(a->st->info) / 2 - 0.5);
				}

				if (esteOperatorSlash(a->dr->info))
				{
					lungimeNumarator = lungimeEcuatie(a->dr->st);

					lungimeNumitor = lungimeEcuatie(a->dr->dr);

					lungimeSt = (float)max(lungimeNumarator, lungimeNumitor) / 2;

					repartizareNoduriArboreExpresiePeColoane(a->dr, coloanaCurenta + lungimeSt + 0.5);
				}
				else // a->st->info este +, -, * sau operand
				{
					lungimeSt = lungimeEcuatie(a->dr->st);
					repartizareNoduriArboreExpresiePeColoane(a->dr, coloanaCurenta + lungimeSt + (float)strlen(a->dr->info) / 2 + 0.5);
				}
			}
	}
}
*/

// Am rescris repartizareNoduriArboreExpresiePeColoane() pentru situatia in care am adaugat si paranteze
void repartizareNoduriArboreExpresiePeColoane(arboreE& a, float coloanaCurenta)
{
	if (a != NULL)
	{
		float lungimeDr, lungimeSt;
		int lungimeNumarator, lungimeNumitor;
		a->coloana = coloanaCurenta;
		if (esteOperator(a->info)) // a->st si a->dr sunt operatori sau operanzi
			if (esteOperatorSlash(a->info))
			{
				if (esteOperatorSlash(a->st->info))
					repartizareNoduriArboreExpresiePeColoane(a->st, coloanaCurenta);
				else // +, -, *, ^, operand
				{
					lungimeSt = lungimeEcuatie(a->st->st);
					lungimeDr = lungimeEcuatie(a->st->dr);
					//repartizareNoduriArboreExpresiePeColoane(a->st, coloanaCurenta + (float)a->numarColoaneLinieFractie / 2 - lungimeDr - 0.5);
					repartizareNoduriArboreExpresiePeColoane(a->st, coloanaCurenta + (lungimeSt - lungimeDr) / 2);
				}
				if (esteOperatorSlash(a->dr->info))
					repartizareNoduriArboreExpresiePeColoane(a->dr, coloanaCurenta);
				else // +, -, *, ^, operand
				{
					lungimeSt = lungimeEcuatie(a->dr->st);
					lungimeDr = lungimeEcuatie(a->dr->dr);
					//repartizareNoduriArboreExpresiePeColoane(a->dr, coloanaCurenta - (float)a->numarColoaneLinieFractie / 2 + lungimeSt + 0.5);
					repartizareNoduriArboreExpresiePeColoane(a->dr, coloanaCurenta + (lungimeSt - lungimeDr) / 2);
				}
			}
			else // a->info este +, -, *, ^
			{
				if (esteOperatorSlash(a->st->info))
				{
					lungimeNumarator = lungimeEcuatie(a->st->st);

					lungimeNumitor = lungimeEcuatie(a->st->dr);

					lungimeDr = (float)max(lungimeNumarator, lungimeNumitor) / 2;

					repartizareNoduriArboreExpresiePeColoane(a->st, coloanaCurenta - lungimeDr - 0.5);
				}
				else // a->st->info este +, -, *, ^ sau operand
				{
					lungimeDr = lungimeEcuatie(a->st->dr);
					repartizareNoduriArboreExpresiePeColoane(a->st, coloanaCurenta - lungimeDr - (float)strlen(a->st->info) / 2 - 0.5);
				}

				if (esteOperatorSlash(a->dr->info))
				{
					lungimeNumarator = lungimeEcuatie(a->dr->st);

					lungimeNumitor = lungimeEcuatie(a->dr->dr);

					lungimeSt = (float)max(lungimeNumarator, lungimeNumitor) / 2;

					repartizareNoduriArboreExpresiePeColoane(a->dr, coloanaCurenta + lungimeSt + 0.5);
				}
				else // a->st->info este +, -, *, ^ sau operand
				{
					lungimeSt = lungimeEcuatie(a->dr->st);
					repartizareNoduriArboreExpresiePeColoane(a->dr, coloanaCurenta + lungimeSt + (float)strlen(a->dr->info) / 2 + 0.5);
				}
			}
		else // a->info este operand sau paranteza si este posibil sa avem paranteza deschisa (sau functie) in fiul stang sau paranteza inchisa in fiul drept
		{
			if (a->st)
				repartizareNoduriArboreExpresiePeColoane(a->st, a->coloana - (float)strlen(a->info) / 2 - (float)strlen(a->st->info) / 2);
			if (a->dr)
				repartizareNoduriArboreExpresiePeColoane(a->dr, a->coloana + (float)strlen(a->info) / 2 + 0.5);

		}
	}
}

void determinareAbatereAfisareEcuatie(arbore a, int& minimNegativLinie, int& minimNegativColoana)
{
	if (a)
	{
		minimNegativLinie = min(minimNegativLinie, a->linie);
		minimNegativColoana = min(minimNegativColoana, a->coloana);
		determinareAbatereAfisareEcuatie(a->st, minimNegativLinie, minimNegativColoana);
		determinareAbatereAfisareEcuatie(a->dr, minimNegativLinie, minimNegativColoana);
	}
}

void determinareAbatereAfisareEcuatie(arboreE a, int& minimNegativLinie, float& minimNegativColoana)
{
	if (a)
	{
		minimNegativLinie = min(minimNegativLinie, a->linie);
		minimNegativColoana = min(minimNegativColoana, a->coloana - (float)strlen(a->info) / 2); // In iteratiile noi, pentru a evita ca entitatile din vestul plansei sa iasa in afara acesetia, am scazut si (float)strlen(a->info) / 2 din coloana entitatii
		determinareAbatereAfisareEcuatie(a->st, minimNegativLinie, minimNegativColoana);
		determinareAbatereAfisareEcuatie(a->dr, minimNegativLinie, minimNegativColoana);
	}
}

void corectieAbatereAfisareEcuatie(arbore& a, int minimNegativLinie, int minimNegativColoana)
{
	if (a)
	{
		a->linie += abs(minimNegativLinie);
		a->coloana += abs(minimNegativColoana);
		corectieAbatereAfisareEcuatie(a->st, minimNegativLinie, minimNegativColoana);
		corectieAbatereAfisareEcuatie(a->dr, minimNegativLinie, minimNegativColoana);
	}
}

void corectieAbatereAfisareEcuatie(arboreE& a, int minimNegativLinie, float minimNegativColoana) //  minimNegativColoana poate fi fractionar pentru iteratiile noi
{
	if (a)
	{
		a->linie += abs(minimNegativLinie);
		a->coloana += fabs(minimNegativColoana); // Poate fi fractionar pentru iteratiile noi
		corectieAbatereAfisareEcuatie(a->st, minimNegativLinie, minimNegativColoana);
		corectieAbatereAfisareEcuatie(a->dr, minimNegativLinie, minimNegativColoana);
	}
}

void actualizareNumarColoaneFractieInArboreExpresie(arbore& a)
{
	if (a)
	{
		if (a->info == '/')
			a->numarColoaneLinieFractie = max(lungimeEcuatie(a->st), lungimeEcuatie(a->dr));
		actualizareNumarColoaneFractieInArboreExpresie(a->st);
		actualizareNumarColoaneFractieInArboreExpresie(a->dr);
	}
}

void actualizareNumarColoaneFractieInArboreExpresie(arboreE& a)
{
	if (a)
	{
		if (esteOperatorSlash(a->info))
			a->numarColoaneLinieFractie = max(lungimeEcuatie(a->st), lungimeEcuatie(a->dr));
		actualizareNumarColoaneFractieInArboreExpresie(a->st);
		actualizareNumarColoaneFractieInArboreExpresie(a->dr);
	}
}

// -----------------------
// Afisare grafica expresie (cu ajutorul grafului expresiei cu locatiile entitatilor actualizate)
// -----------------------

void determinareNumarLiniiColoanePlansaEcuatie(arbore a, int& maxPozitivLinie, int& maxPozitivColoana)
{
	if (a)
	{
		maxPozitivLinie = max(maxPozitivLinie, a->linie);
		maxPozitivColoana = max(maxPozitivColoana, a->coloana);
		determinareNumarLiniiColoanePlansaEcuatie(a->st, maxPozitivLinie, maxPozitivColoana);
		determinareNumarLiniiColoanePlansaEcuatie(a->dr, maxPozitivLinie, maxPozitivColoana);
	}
}

void determinareNumarLiniiColoanePlansaEcuatie(arboreE a, int& maxPozitivLinie, float& maxPozitivColoana)
{
	if (a)
	{
		maxPozitivLinie = max(maxPozitivLinie, a->linie);
		maxPozitivColoana = max(maxPozitivColoana, a->coloana + (float)strlen(a->info) / 2 - 1); // maxPozitivColoana poate fi fractionar pentru iteratiile noi
		determinareNumarLiniiColoanePlansaEcuatie(a->st, maxPozitivLinie, maxPozitivColoana);
		determinareNumarLiniiColoanePlansaEcuatie(a->dr, maxPozitivLinie, maxPozitivColoana);
	}
}

void afisareElementeArboreExpresieSRD(arbore a)
{
	if (a)
	{
		afisareElementeArboreExpresieSRD(a->st);

		cout << '(' << a->info << ',' << a->linie << ',' << a->coloana << ',' << a->numarColoaneLinieFractie << ")\n";

		afisareElementeArboreExpresieSRD(a->dr);
	}
}

void afisareElementeArboreExpresieSRD(arboreE a)
{
	if (a)
	{
		afisareElementeArboreExpresieSRD(a->st);

		cout << '(' << a->info << ',' << a->linie << ',' << a->coloana << ',' << a->numarColoaneLinieFractie << ")\n";

		afisareElementeArboreExpresieSRD(a->dr);
	}
}

void afisareExpresie(arbore nodCurent, int lungimeCelula, int inaltimeCelula)
{
	if (nodCurent != NULL)
	{
		int x = nodCurent->coloana * lungimeCelula + lungimeCelula / 2;
		int y = nodCurent->linie * inaltimeCelula + inaltimeCelula / 2;
		int marimeFont = 7;
		//marimeFont = min(lungimeCelula, inaltimeCelula);
		settextstyle(8, 0, marimeFont);
		char element[50];
		element[0] = nodCurent->info;
		element[1] = '\0';
		if (nodCurent->info != '/')
			outtextxy(x - textwidth(element) / 2, y - textheight(element) / 2, element);
		else
		{
			//line(x - lungimeCelula / 2, y, x + lungimeCelula / 2, y);
			line(x - (float)nodCurent->numarColoaneLinieFractie / 2 * lungimeCelula, y, x + (float)nodCurent->numarColoaneLinieFractie / 2 * lungimeCelula, y);///NOU
		}
		afisareExpresie(nodCurent->st, lungimeCelula, inaltimeCelula);
		afisareExpresie(nodCurent->dr, lungimeCelula, inaltimeCelula);
	}
}

void afisareExpresie(arboreE nodCurent, float lungimeCelula, float inaltimeCelula) // real pentru o precizie mai buna in iteratiile noi
{
	if (nodCurent != NULL)
	{
		float x = nodCurent->coloana * lungimeCelula; // real pentru o precizie mai buna in iteratiile noi
		float y = nodCurent->linie * inaltimeCelula + inaltimeCelula / 2; // real pentru o precizie mai buna in iteratiile noi
		int marimeFont;
		marimeFont = min(min((int)lungimeCelula, (int)inaltimeCelula) / 8, 10);
		settextstyle(8, 0, marimeFont);
		if (!esteOperatorSlash(nodCurent->info))
		{
			outtextxy(x - (float)textwidth(nodCurent->info) / 2, y - (float)textheight(nodCurent->info) / 2, nodCurent->info);
			//cout << "entitate: " << x - (float)textwidth(nodCurent->info) / 2 << ' ' << y - (float)textheight(nodCurent->info) / 2 << '\n';
		}
		else
		{
			//line(x - lungimeCelula / 2, y, x + lungimeCelula / 2, y);
			line(x - (float)nodCurent->numarColoaneLinieFractie / 2 * lungimeCelula, y, x + (float)nodCurent->numarColoaneLinieFractie / 2 * lungimeCelula, y);
			//cout << "linie X: " << x - (float)nodCurent->numarColoaneLinieFractie / 2 * lungimeCelula << " " << x + (float)nodCurent->numarColoaneLinieFractie / 2 * lungimeCelula << '\n';
		}
		afisareExpresie(nodCurent->st, lungimeCelula, inaltimeCelula);
		afisareExpresie(nodCurent->dr, lungimeCelula, inaltimeCelula);
	}
}

void separator()
{
	cout << "\n\n";
	for (int i = 0; i < 50; i++)
		cout << char(175);
	cout << '|';
	for (int i = 0; i < 50; i++)
		cout << char(174);
	cout << "\n\n\n";
	afisareLiniute();
}

void cod_pt_arb_fara_paranteze()
{
	int windowArbE;
	int lungimePlansa = 900, inaltimePlansa = 600; // Comune pentru iteratii noi si vechi
	windowArbE = initwindow(lungimePlansa, inaltimePlansa, "windowArbE");

	setcolor(Culori[index_culoareArbore].culoare);
	setcurrentwindow(windowArbE);
	afisareArbore(_a, NULL, _lungimeCelula, _inaltimeCelula, _razaCerc);
}

void cod_pt_arb_cu_paranteze()
{
	int windowArbPE;
	int lungimePlansa = 900, inaltimePlansa = 600; // Comune pentru iteratii noi si vechi
	windowArbPE = initwindow(lungimePlansa, inaltimePlansa, "windowArbEP");

	setcolor(Culori[index_culoareArbore].culoare);
	setcurrentwindow(windowArbPE);
	afisareArbore(_b, NULL, _lungimeCelulaArboreCuParanteze, _inaltimeCelulaArboreCuParanteze, _razaCercArboreCuParanteze);
}

void cod_pt_afisare_expresie()
{
	int  windowEcPE;
	int lungimePlansa = 900, inaltimePlansa = 600; // Comune pentru iteratii noi si vechi
	windowEcPE = initwindow(lungimePlansa, inaltimePlansa, "windowEcPE");

	setcolor(Culori[index_culoareEcuatie].culoare);
	setcurrentwindow(windowEcPE);
	afisareExpresie(_bExpresie, _lungimeCelulaExpresieCuParanteze, _inaltimeCelulaExpresieCuParanteze);
}

void main2()
{
	// -----------------------
	// Initializarea bibliotecii grafice si citirea expresiei matematice
	// -----------------------

	//int gd = DETECT, gm;
	//initgraph(&gd, &gm, NULL);

	//freopen("proiect.in", "r", stdin);
	//cin >> infix;

	// -----------------------
	// Ferestrele pentru reprezentarile grafice (iteratii vechi si noi)
	// -----------------------

	//int windowArb, windowArbP, windowEc, windowEcP; //Versiuni vechi (iteratia 1)
	//windowArb = initwindow(lungimePlansa, inaltimePlansa, "windowArb");
	//windowArbP = initwindow(lungimePlansa, inaltimePlansa, "windowArbP");
	//windowEc = initwindow(lungimePlansa, inaltimePlansa, "windowEc");
	//windowEcP = initwindow(lungimePlansa, inaltimePlansa, "windowEcP");

	// -----------------------
	// Iteratii noi (cu matrice de caractere / arbori pentru care cheile sunt entitati - operanzii pot avea orice lungime)
	// -----------------------

	ecuatieStringCatreTablouDeEntitati(infixString, _infix); // Parsare char*
	afisareTablouEntitati(_infix);
	afisareInfixCreareSiAfisarePostfixPrefix(_infix, _postfix, _prefix);

	construireSiActualizareArbore(_a, lungimePlansa, inaltimePlansa, _lungimeCelula, _inaltimeCelula, _razaCerc, _prefix); // Arborele prefixului



	duplicareArbore(_a, _b);
	//adaugareParanteze(_b);
	adaugareParanteze(_b, NULL); // Pentru iteratiile de ordin 3 in care am pus paranteze doar unde era necesar

	actualizareArboreCuParanteze(_b, lungimePlansa, inaltimePlansa, _lungimeCelulaArboreCuParanteze, _inaltimeCelulaArboreCuParanteze, _razaCercArboreCuParanteze);



	afisareInaltimeEcuatie(_nrLiniiEcuatie = inaltimeEcuatie(_b));

	afisareLungimeEcuatie(_nrColoaneEcuatie = lungimeEcuatie(_b));

	duplicareArbore(_a, _aExpresie);

	// Pentru iteratiile de ordin 2 ale proiectului
	/*
	intitializareLiniiColoaneCuInfinit(_aExpresie);
	repartizareNoduriArboreExpresiePeLiniiFractie(_aExpresie, -2, _linieUrmatoareaFractie, 0, false);
	repartizareNoduriArboreExpresiePeLiniiNumaratorNumitor(_aExpresie, 0);
	*/

	// Pentru iteratiile de ordin 3 ale proiectului (2 in 1, optimizare de cod si reparare bug fractii supraetajate)
	repartizareNoduriArboreExpresiePeLinii(_aExpresie, NULL, 0);

	actualizareNumarColoaneFractieInArboreExpresie(_aExpresie); // Am nevoie de aceasta actualizare in repartizareNoduriArboreExpresiePeColoane()

	repartizareNoduriArboreExpresiePeColoane(_aExpresie, 0);

	_minimNegativLinie = _minimNegativColoana = INFINIT;
	determinareAbatereAfisareEcuatie(_aExpresie, _minimNegativLinie, _minimNegativColoana);
	cout << "minimNegativLinie este: " << _minimNegativLinie << ", iar minimNegativColoana este: " << _minimNegativColoana << '\n';
	afisareLiniute();
	corectieAbatereAfisareEcuatie(_aExpresie, _minimNegativLinie, _minimNegativColoana);

	_maxPozitivLinie = _maxPozitivColoana = -INFINIT;
	determinareNumarLiniiColoanePlansaEcuatie(_aExpresie, _maxPozitivLinie, _maxPozitivColoana);
	_numarLiniiPlansaEcuatie = _maxPozitivLinie + 1;
	_numarColoanePlansaEcuatie = _maxPozitivColoana + 1;

	cout << "Numarul (efectiv) de linii ale plansei ecuatiei (in arborele fara paranteze adaugate): " << _numarLiniiPlansaEcuatie << '\n';
	afisareLiniute();
	cout << "Numarul (efectiv) de coloane ale plansei ecuatiei (in arborele fara paranteze adaugate): " << _numarColoanePlansaEcuatie << '\n';
	afisareLiniute();

	cout << "(Element,linie,colana,numarColoaneFractie) in arborele fara paranteze adugate:\n";
	afisareElementeArboreExpresieSRD(_aExpresie);
	afisareLiniute();

	_lungimeCelulaExpresie = (float)lungimePlansa / _numarColoanePlansaEcuatie; // real pentru o precizie mai buna
	_inaltimeCelulaExpresie = (float)inaltimePlansa / _numarLiniiPlansaEcuatie; // real pentru o precizie mai buna
	//setcurrentwindow(windowEcE);
	//afisareExpresie(_aExpresie, _lungimeCelulaExpresie, _inaltimeCelulaExpresie);

	// (NOU) In iteratia 3 am reprezentat ecuatia cu paranteze adaugate:

	//duplicareArbore(_b, _bExpresie);
	duplicareArbore(_a, _bExpresie); // Vom duplica arborele fara paranteze (_a) deoarece acestea vor fi adaugate din nou, cu linia operatorului pentru care au fost introduse

	repartizareNoduriArboreExpresiePeLinii(_bExpresie, NULL, 0);

	adaugareParanteze(_bExpresie, NULL); // Am adaugat parantezele intre actualizarea liniilor pentru entitati si actualizarea coloanelor pentru entitati

	/*
	cout << "$$$" << inaltimeEcuatieLaNumaratori(_bExpresie->dr) << "$$$\n";
	cout << "$$$" << inaltimeEcuatieLaNumitori(_bExpresie->dr) << "$$$\n";
	*/
	cout << "$$$\n";
	SRD(_bExpresie);
	cout << "\n$$$\n";

	actualizareNumarColoaneFractieInArboreExpresie(_bExpresie); // Am nevoie de aceasta actualizare in repartizareNoduriArboreExpresiePeColoane()

	actualizareArboreExpresieCuFunctii(_bExpresie, NULL, NULL);
	actualizareArboreExpresieCuFuctiiInRadacina(_bExpresie);

	repartizareNoduriArboreExpresiePeColoane(_bExpresie, 0);

	cout << "$$$\n";
	//SRD(_bExpresie);
	afisareElementeArboreExpresieSRD(_bExpresie);
	cout << "\n$$$\n";

	_minimNegativLinieEcuatieCuParanteze = _minimNegativColoanaEcuatieCuParanteze = INFINIT;
	determinareAbatereAfisareEcuatie(_bExpresie, _minimNegativLinieEcuatieCuParanteze, _minimNegativColoanaEcuatieCuParanteze);
	cout << "minimNegativLinieEcuatieCuParanteze este: " << _minimNegativLinieEcuatieCuParanteze << ", iar minimNegativColoanaEcuatieCuParanteze este: " << _minimNegativColoanaEcuatieCuParanteze << '\n';
	afisareLiniute();
	corectieAbatereAfisareEcuatie(_bExpresie, _minimNegativLinieEcuatieCuParanteze, _minimNegativColoanaEcuatieCuParanteze);

	_maxPozitivLinieEcuatieCuParanteze = _maxPozitivColoanaEcuatieCuParanteze = -INFINIT;
	determinareNumarLiniiColoanePlansaEcuatie(_bExpresie, _maxPozitivLinieEcuatieCuParanteze, _maxPozitivColoanaEcuatieCuParanteze);
	_numarLiniiPlansaEcuatieCuParanteze = _maxPozitivLinieEcuatieCuParanteze + 1;
	_numarColoanePlansaEcuatieCuParanteze = ceil(_maxPozitivColoanaEcuatieCuParanteze) + 1;

	cout << "Numarul (efectiv) de linii ale plansei ecuatiei (in arborele CU paranteze adaugate): " << _numarLiniiPlansaEcuatieCuParanteze << '\n';
	afisareLiniute();
	cout << "Numarul (efectiv) de coloane ale plansei ecuatiei (in arborele CU paranteze adaugate): " << _numarColoanePlansaEcuatieCuParanteze << '\n';
	afisareLiniute();

	cout << "(Element,linie,colana,numarColoaneFractie) in arborele CU paranteze adugate:\n";
	afisareElementeArboreExpresieSRD(_bExpresie);
	afisareLiniute();

	_lungimeCelulaExpresieCuParanteze = (float)lungimePlansa / _numarColoanePlansaEcuatieCuParanteze; // real pentru o precizie mai buna
	_inaltimeCelulaExpresieCuParanteze = (float)inaltimePlansa / _numarLiniiPlansaEcuatieCuParanteze; // real pentru o precizie mai buna

	//cout << "$$$ " << _lungimeCelulaExpresie << " " << _inaltimeCelulaExpresie << " $$$\n";
	//cout << "$$$ " << lungimeEcuatie(_aExpresie->st) << " - " << lungimeEcuatie(_aExpresie->dr) << " $$$\n";

	// -----------------------
	// Separator
	// -----------------------


	/*
	separator();

	// -----------------------
	// Iteratii vechi (arbori pentru care cheile sunt caractere - operanzii pot avea doar lungimea 1)
	// -----------------------

	afisareInfixCreareSiAfisarePostfixPrefix(infix, postfix, prefix);

	prefixCatreArboreTablou(0, 1, dimensiuneTablouri, prefix, arboreTablou);
	umplereCuSemnulIntrebarii(arboreTablou, dimensiuneTablouri);
	afiseazaPozitiaInArboreSiCaracterulDinNod(arboreTablou, dimensiuneTablouri);

	construireVectorDeTati(arboreTablou, T, dimensiuneTablouri);
	construireVectorFiuStSiFiuDr(T, fiu_st, fiu_dr, dimensiuneTablouri);
	afisareLiniute();

	afisareVectoriDeTati(T, dimensiuneTablouri);
	afisareVectorDeFiiStangi(fiu_st, dimensiuneTablouri);
	afisareVectorDeFiiDrepti(fiu_dr, dimensiuneTablouri);

	construireSiActualizareArbore(a, T, fiu_st, fiu_dr, lungimePlansa, inaltimePlansa, lungimeCelula, inaltimeCelula, razaCerc, arboreTablou, dimensiuneTablouri);

	setcurrentwindow(windowArb);
	afisareArbore(a, NULL, lungimeCelula, inaltimeCelula, razaCerc);

	duplicareArbore(a, b);
	adaugareParanteze(b);

	actualizareArboreCuParanteze(b, lungimePlansa, inaltimePlansa, lungimeCelulaArboreCuParanteze, inaltimeCelulaArboreCuParanteze, razaCercArboreCuParanteze);
	setcurrentwindow(windowArbP);
	afisareArbore(b, NULL, lungimeCelulaArboreCuParanteze, inaltimeCelulaArboreCuParanteze, razaCercArboreCuParanteze);

	cout << "(Pentru expresii cu operanzi de lungime 1) ";
	afisareInaltimeEcuatie(nrLiniiEcuatie = inaltimeEcuatie(b));

	cout << "(Pentru expresii cu operanzi de lungime 1) ";
	afisareLungimeEcuatie(nrColoaneEcuatie = lungimeEcuatie(b));

	duplicareArbore(a, aExpresie);
	intitializareLiniiColoaneCuInfinit(aExpresie);

	repartizareNoduriArboreExpresiePeLiniiFractie(aExpresie, -2, linieUrmatoareaFractie, 0, false);
	repartizareNoduriArboreExpresiePeLiniiNumaratorNumitor(aExpresie, 0);
	repartizareNoduriArboreExpresiePeColoane(aExpresie, 0);

	minimNegativLinie = minimNegativColoana = INFINIT;
	determinareAbatereAfisareEcuatie(aExpresie, minimNegativLinie, minimNegativColoana);
	cout << "(Pentru expresii cu operanzi de lungime 1) minimNegativLinie este: " << minimNegativLinie << ", iar minimNegativColoana este: " << minimNegativColoana << '\n';
	afisareLiniute();
	corectieAbatereAfisareEcuatie(aExpresie, minimNegativLinie, minimNegativColoana);

	maxPozitivLinie = maxPozitivColoana = -INFINIT;
	determinareNumarLiniiColoanePlansaEcuatie(aExpresie, maxPozitivLinie, maxPozitivColoana);
	numarLiniiPlansaEcuatie = maxPozitivLinie + 1;
	numarColoanePlansaEcuatie = maxPozitivColoana + 1;

	cout << "(Pentru expresii cu operanzi de lungime 1) Numarul (efectiv) de linii ale plansei ecuatiei (in arborele fara paranteze adaugate): " << numarLiniiPlansaEcuatie << '\n';
	afisareLiniute();
	cout << "(Pentru expresii cu operanzi de lungime 1) Numarul (efectiv) de coloane ale plansei ecuatiei (in arborele fara paranteze adaugate): " << numarColoanePlansaEcuatie << '\n';
	afisareLiniute();

	actualizareNumarColoaneFractieInArboreExpresie(aExpresie);

	cout << "(Pentru expresii cu operanzi de lungime 1) (Element,linie,colana,numarColoaneFractie) in arborele fara paranteze adugate:\n";
	afisareElementeArboreExpresieSRD(aExpresie);
	afisareLiniute();

	lungimeCelulaExpresie = lungimePlansa / numarColoanePlansaEcuatie;
	inaltimeCelulaExpresie = inaltimePlansa / numarLiniiPlansaEcuatie;
	setcurrentwindow(windowEc);
	afisareExpresie(aExpresie, lungimeCelulaExpresie, inaltimeCelulaExpresie);

	setcurrentwindow(windowEcP);
	// Aici au fost oprite din dezvoltare iteratiile vechi (in care operanzii aveau lungimea 1)

	*/

}


/// COD MENIU INCEPUT


bool ruleaza = true;
int  MeniuGraficaFereastra;
int pozitieCaracter = 0;
const int background_color = COLOR(25, 25, 36); ///181824
const int button_color = COLOR(226, 6, 76); ///E2064C
const int text_color = COLOR(255, 255, 255);

enum TipFereastra
{
	MainMenu,
	Setari,
	Ajutor,
	Start,
	Afisari
};

TipFereastra fereastraCurenta;

void square(int x, int y, int latura)
{
	rectangle(x - latura / 2, y - latura / 2, x + latura / 2, y + latura / 2);
}

void clearScreen()
{
	cleardevice();
	setfillstyle(SOLID_FILL, background_color);
	bar(0, 0, getmaxx(), getmaxy());
	setbkcolor(background_color);
}

bool peButon(int x1, int y1, int x2, int y2)
{
	int x = mousex();
	int y = mousey();

	if (x1 > x2)
		swap(x1, x2);

	if (y1 > y2)
		swap(y1, y2);


	if (x > x1 && x<x2 && y>y1 && y < y2)
		return true;

	return false;
}

void underlineText(int x, int y, char* text)
{
	// Obtine latimea textului in pixeli
	int text_width = textwidth(text);

	// Obtine inaltimea textului in pixeli
	int text_height = textheight(text);

	// Desenam textul in mijlocul dreptunghiului
	line(x - text_width / 2, y + text_height + 2, x + text_width / 2, y + text_height + 2);
}

void drawCenteredText(int left, int top, int right, int bottom, char* text)
{
	// Obtine latimea textului in pixeli
	int text_width = textwidth(text);

	// Calculam coordonata x pentru centrul dreptunghiului
	int x = (right - left - text_width) / 2 + left;

	// Calculam coordonata y pentru centrul dreptunghiului
	int y = (bottom - top) / 2 + top;

	// Desenam textul in mijlocul dreptunghiului
	setcolor(text_color);
	outtextxy(x, y, text);
}

void afisareTextAjutor()
{
	outtextxy(0, 120, " !!! Reguli pentru introducerea expresiei:");
	outtextxy(0, 160, "    --Toate caracterele vor fi litere mici, fara diacritice, ");
	outtextxy(0, 180, " legate (fara spatii)");


	outtextxy(0, 220, "   --Functiile si Operatiile suportate sunt inmultire, impartire,");
	outtextxy(0, 240, " adunare, scadere, numere reale, sinus, cosinus, tangenta,");
	outtextxy(0, 260, " cotangenta.");

	outtextxy(0, 300, "   -- Exemple de expresii :  32+(0.5)*sin(x+2)/4/2+32");


	outtextxy(0, 340, "   In interfeata Start veti vedea un camp destinat locului in care sa ");
	outtextxy(0, 360, " introduceti ecuatia matematica.");

	setcolor(LIGHTRED);

	outtextxy(0, 400, "   !!!!  Dupa ce ati introdus expresia, apasati tasta enter iar ");
	outtextxy(0, 420, " ulterior butonul Confirm.");

	setcolor(WHITE);

	outtextxy(0, 460, "   Dupa ce ati efectuat pasii de mai sus, puteti selecta sa vizualizati");
	outtextxy(0, 480, " arborele asociat expresiei ( cu sau fara paranteze) sau afisarea");
	outtextxy(0, 500, " expresiei matematice.");

}


/// *************************************************************************************

void setari()
{
	/// Buton setari
	setcolor(button_color);
	rectangle(50, getmaxy() - 50, 150, getmaxy() - 150);
	drawCenteredText(50, getmaxy() - 50, 150, getmaxy() - 150, "Settings");
}

bool peSetari()
{
	if (peButon(50, getmaxy() - 50, 150, getmaxy() - 150))
		return true;
	return false;
}

void quit()
{
	/// Buton Quit
	setcolor(button_color);
	rectangle(getmaxx() - 50, getmaxy() - 50, getmaxx() - 150, getmaxy() - 150);
	drawCenteredText(getmaxx() - 50, getmaxy() - 50, getmaxx() - 150, getmaxy() - 150, "Quit");
}

bool peQuit()
{
	if (peButon(getmaxx() - 50, getmaxy() - 50, getmaxx() - 150, getmaxy() - 150))
		return true;
	return false;
}

void ajutor()
{
	/// Buton Ajutor
	setcolor(button_color);
	int mijlocX = getmaxx() / 2;
	rectangle(mijlocX - 50, getmaxy() - 50, mijlocX + 50, getmaxy() - 150);
	drawCenteredText(mijlocX - 50, getmaxy() - 50, mijlocX + 50, getmaxy() - 150, "Ajutor");
}

bool peAjutor()
{
	int mijlocX = getmaxx() / 2;
	if (peButon(mijlocX - 50, getmaxy() - 50, mijlocX + 50, getmaxy() - 150))
		return true;
	return false;
}

void start()
{
	/// Buton Start
	setcolor(button_color);
	int mijlocX = getmaxx() / 2;
	int mijlocY = getmaxy() / 2;
	rectangle(mijlocX - 50, mijlocY - 50, mijlocX + 50, mijlocY + 50);
	drawCenteredText(mijlocX - 50, mijlocY - 50, mijlocX + 50, mijlocY + 50, "Start");
}

bool peStart()
{
	int mijlocX = getmaxx() / 2;
	int mijlocY = getmaxy() / 2;
	if (peButon(mijlocX - 50, mijlocY - 50, mijlocX + 50, mijlocY + 50))
		return true;
	return false;
}

void titlu()
{
	/// titlu proiect
	int text_width = textwidth("Editor Expresii Matematice");
	int x = (getmaxx() - text_width) / 2;
	outtextxy(x, 50, "Editor Expresii Matematice");
}

/// *************************************************************************************

void InapoiLaMeniu()
{
	/// Inapoi La Meniu
	setcolor(button_color);
	rectangle(20, 20, 240, 60);
	drawCenteredText(20, 20, 240, 60, "<-- Inapoi La Meniu");
}

bool peInapoiLaMeniu()
{
	if (peButon(20, 20, 240, 60))
		return true;
	return false;
}

void bifeazaSauDebifeazaEcuatie(int indexButon)
{
	for (int i = 0; i < 6; i++)
	{
		if (i == indexButon)
			setfillstyle(SOLID_FILL, Culori[i].culoare);
		else
			setfillstyle(SOLID_FILL, background_color);
		/// creez un patrat negru
		bar(490, 230 + i * 40, 510, 250 + i * 40);
		/// creez conturu colorat
		setcolor(Culori[i].culoare);
		square(500, 240 + i * 40, 20);
	}
	setcolor(WHITE);
}

void butoaneCuloriEcuatie()
{
	drawCenteredText(600, 180, 600, 180, "Culoare Ecuatie");
	underlineText(600, 180, "Culoare Ecuatie");

	bifeazaSauDebifeazaEcuatie(index_culoareEcuatie);
	for (int i = 0; i < 6; i++)
	{
		setcolor(Culori[i].culoare);
		outtextxy(540, 240 + i * 40 - 5, Culori[i].nume);
	}
	setcolor(WHITE);
}

/// verifica daca mouse ul e pe butoane si face actiunea butonului respectiv
void procesezButonCuloareEcuatie()
{
	for (int i = 0; i < 6; i++)
	{
		if (peButon(490, 230 + i * 40, 510, 250 + i * 40))
		{
			/// !!! functia care mi seteaza culoarea ecuatiei: culoareaEcuatie=Culori[i].culoare;  ///setcolor (Culori[index_culoareEcuatie].culoare)
			index_culoareEcuatie = i;
			cout << "Culoarea aleasa ecuatiei este culoarea: " << Culori[i].nume << endl;
			bifeazaSauDebifeazaEcuatie(i);
		}
	}
}


///===================================================================================

void bifeazaSauDebifeazaArbore(int indexButon)
{
	for (int i = 0; i < 6; i++)
	{
		if (i == indexButon)
			setfillstyle(SOLID_FILL, Culori[i].culoare);
		else
			setfillstyle(SOLID_FILL, background_color);
		/// creez un patrat negru
		bar(90, 230 + i * 40, 110, 250 + i * 40);
		/// creez conturu colorat
		setcolor(Culori[i].culoare);
		square(100, 240 + i * 40, 20);
	}
	setcolor(WHITE);
}

void butoaneCuloriArbore()
{
	drawCenteredText(200, 180, 200, 180, "Culoare Arbore");
	underlineText(200, 180, "Culoare Arbore");

	bifeazaSauDebifeazaArbore(index_culoareArbore);
	for (int i = 0; i < 6; i++)
	{
		setcolor(Culori[i].culoare);
		outtextxy(140, 240 + i * 40 - 5, Culori[i].nume);
	}
	setcolor(WHITE);
}

/// verifica daca mouse ul e pe butoane si face actionea butonului respectiv
void procesezButonCuloareArbore()
{
	for (int i = 0; i < 6; i++)
	{
		if (peButon(90, 230 + i * 40, 110, 250 + i * 40))
		{
			/// !!! functia care mi seteaza culoarea arborelui: culoareaArborelui=Culori[i].culoare;
			index_culoareArbore = i;
			cout << "Culoarea aleasa este culoarea: " << Culori[i].nume << endl;
			bifeazaSauDebifeazaArbore(i);
		}
	}
}

///===================================================================================

void deseneazaFereastraAjutor()
{
	fereastraCurenta = Ajutor;
	InapoiLaMeniu();
	afisareTextAjutor();
}

void deseneazaFereastraSetari()
{
	fereastraCurenta = Setari;
	butoaneCuloriArbore();
	butoaneCuloriEcuatie();
	InapoiLaMeniu();
}

void ButonConfirm()
{
	/// Buton Confirm
	setcolor(button_color);
	int mijlocX = getmaxx() / 2;
	rectangle(mijlocX - 50, 280, mijlocX + 50, 380);
	drawCenteredText(mijlocX - 50, 280, mijlocX + 50, 380, "Confirm");
}

bool peButonConfirm()
{
	int mijlocX = getmaxx() / 2;
	if (peButon(mijlocX - 50, 280, mijlocX + 50, 380))
		return true;
	return false;
}

void CasutaText()
{
	int text_width = textwidth("Introduceti expresia matematica mai jos:");
	int x = (getmaxx() - text_width) / 2;
	outtextxy(x, 100, "Introduceti expresia matematica mai jos:");

	setcolor(button_color);
	rectangle(80, 140, 720, 200);
}


void deseneazaFereastraStart()
{
	fereastraCurenta = Start;
	InapoiLaMeniu();
	CasutaText();
	ButonConfirm();
	int text_width = 0;
	int nrCaractere = 0;

	char c[2];
	c[1] = '\0';
	while (c[0] != 13)
	{
		c[0] = getch();
		if (c[0] == 13)
			break;
		if (c[0] == 8 && nrCaractere > 0)
		{
			pozitieCaracter = pozitieCaracter - text_width - 2;
			infixString[strlen(infixString) - 1] = '\0';
			outtextxy(90 + pozitieCaracter, 170, " ");
			nrCaractere--;
		}
		else if (c[0] != 8)
		{
			nrCaractere++;
			strncat(infixString, c, 1);
			outtextxy(90 + pozitieCaracter, 170, c);
			text_width = textwidth(c);
			pozitieCaracter = pozitieCaracter + text_width + 2;
		}
	}
}


/// *************************************************************************************

void deseneazaFereastraMeniuPrincipal()
{
	pozitieCaracter = 0;
	memset(infixString, 0, 50);
	fereastraCurenta = MainMenu;
	titlu();
	start();
	ajutor();
	quit();
	setari();
}

void butonArboreFaraParanteze()
{
	/// Buton Arbore Fara Paranteze
	setcolor(button_color);
	rectangle(80, 140, 720, 200);
	drawCenteredText(80, 140, 720, 200, "Afiseaza arborele expresiei fara paranteze");
}

bool peButonArboreFaraParanteze()
{
	if (peButon(80, 140, 720, 200))
		return true;
	return false;
}

void butonArboreCuParanteze()
{
	/// Buton Arbore Cu Paranteze
	setcolor(button_color);
	rectangle(80, 250, 720, 310);
	drawCenteredText(80, 250, 720, 310, "Afiseaza arborele expresiei cu paranteze");
}

bool peButonArboreCuParanteze()
{
	if (peButon(80, 250, 720, 310))
		return true;
	return false;
}

void butonExpresie()
{
	/// Buton Expresie
	setcolor(button_color);
	rectangle(80, 360, 720, 420);
	drawCenteredText(80, 360, 720, 420, "Afiseaza expresia pe ecran");
}

bool peButonExpresie()
{
	if (peButon(80, 360, 720, 420))
		return true;
	return false;
}

void deseneazaFereastraAfisari()
{
	fereastraCurenta = Afisari;
	InapoiLaMeniu();
	butonArboreFaraParanteze();
	butonArboreCuParanteze();
	butonExpresie();
}

void setariProiect()
{
	MeniuGraficaFereastra = initwindow(800, 600, "Meniu Principal");
	clearScreen();
	setcolor(WHITE);
	settextstyle(DEFAULT_FONT, HORIZ_DIR, 100);
}

void verificaButoane()
{
	switch (fereastraCurenta)
	{
	case MainMenu:
		if (peStart())
		{
			cout << "Ai dat click pe Start" << endl;
			clearScreen();
			deseneazaFereastraStart();
		}

		if (peSetari())
		{
			cout << "Ai dat click pe Setari" << endl;
			clearScreen();
			deseneazaFereastraSetari();
		}

		if (peAjutor())
		{
			clearScreen();
			deseneazaFereastraAjutor();
			cout << "Ai dat click pe Ajutor" << endl;
		}

		if (peQuit())
		{
			cout << "Ai dat click pe Quit" << endl;
			clearScreen();
			ruleaza = 0;
		}
		break;

	case Setari:
		if (peInapoiLaMeniu())
		{
			clearScreen();
			deseneazaFereastraMeniuPrincipal();
		}
		procesezButonCuloareArbore();
		procesezButonCuloareEcuatie();
		break;

	case Ajutor:
		if (peInapoiLaMeniu())
		{
			clearScreen();
			deseneazaFereastraMeniuPrincipal();
		}
		break;

	case Start:
		if (peInapoiLaMeniu())
		{
			clearScreen();
			deseneazaFereastraMeniuPrincipal();
		}
		if (peButonConfirm())
		{
			cout << infix << endl;
			clearScreen();
			deseneazaFereastraAfisari();
			main2();
		}
		break;

	case Afisari:
		if (peInapoiLaMeniu())
		{
			clearScreen();
			deseneazaFereastraMeniuPrincipal();
		}
		if (peButonArboreFaraParanteze())
		{
			cod_pt_arb_fara_paranteze();
		}
		if (peButonArboreCuParanteze())
		{
			cod_pt_arb_cu_paranteze();
		}
		if (peButonExpresie())
		{
			cod_pt_afisare_expresie();
		}
		break;

	}
}

/// COD MENIU TERMINAT

int main()
{
	setariProiect();
	deseneazaFereastraMeniuPrincipal();

	while (ruleaza)
	{
		if (ismouseclick(WM_LBUTTONDOWN))
		{
			clearmouseclick(WM_LBUTTONDOWN);
			verificaButoane();
		}
		setcurrentwindow(MeniuGraficaFereastra);
	}

	closegraph();
	return 0;
}


/**

 ***** Exemple de ecuatii *****

1)     1/2+5/(1/3+1/(4+(1+6)/5))
2)     (1+2)/3+(6+7)/(1/2+5+1/(2+3))
3)     (1+2)/2+(1+2)/n
4)     (234/B+abc)/23+(1000+2)/n
5)     123/2+abc/(21/3400+1/(A+(1+645)/xyz))
6)     sin(345+a)/cos(235-b)+sin(a*b)
7)     ctg(123*a)/(21+xy+555)
8)     123+sin(24.55+abc)/cos(tg(90.99))
9)     sin(9.5+1)/(1+cos(8+ac))


*/


/*
	infixat:         postfixat:
	((a+(b*c))-d) => abc*+d-

	infixat:         prefixat:
	((a+(b*c))-d) => -+a*bcd

	indici:              0 1 2 3 4  5  6  7  8 9 10
	Vectorul de tati: T={0 1 1 2 2 -1 -1 -1 -1 5 5}
			   fiu_st[]={2 4 0 0 10 0  0  0  0 0 0}
			   fiu_dr[]={3 5 0 0 11 0  0  0  0 0 0}
	arbore:
					  -
					 / \
					+   d
				   / \
				  a   *
					 / \
					b   c
*/
