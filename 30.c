#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <stddef.h>
#define len 500

//definisco le strutture

typedef struct ing { //struttura ingredienti
	char nome[30];
    unsigned long cod;
	int quantita;
}t_ingrediente;

typedef struct I { //struttura nodi lista ingredienti
	t_ingrediente ingrediente;
	struct I* next;
}I_nodo;

typedef I_nodo* t_listaingredienti; //definizione lista ingredienti

typedef struct ric { //struttura ricette
	char nome[30];
    unsigned long cod;
	int peso;
	t_listaingredienti listaingredienti;
    struct ric* next;
}t_ricetta;

typedef t_ricetta *ricet;

typedef struct O { //struttura nodi ordine singolo
	ricet ricetta;
	int q;
	int peso;
}t_ordine;

typedef struct A { //struttura nodi lista attesa ordini
	t_ordine ordine;
	int t;
	struct A* next;
}A_nodo;

typedef A_nodo* t_listaordini; //definizione lista attesa ordini singoli

typedef struct L {
	int q;
	int scadenza;
	struct L* next;
}L_nodo;

typedef L_nodo* t_lotti;

typedef struct M { //struttura nodi elenco magazzino
	char ingrediente[30];
    unsigned long cod;
	int peso;
	t_lotti lotto;
    struct M *next;
}t_magazzino;

typedef t_magazzino *mag;

mag magazzino[len]={NULL};
ricet ricettario[len]={NULL};
mag tot[len]={NULL};
char stringa[30]="";

void cancellalistaL(t_lotti lista)
{
	if (lista != NULL)
	{
		cancellalistaL(lista->next);
		free(lista);
	}
}

void cancellalistaI(t_listaingredienti lista)
{
	if (lista != NULL)
	{
		cancellalistaI(lista->next);
		free(lista);
	}
}

unsigned long converti(char *s)
{
    unsigned long hash=5381;
	int c;
	while((c=*s++)) 
	{
		hash=((hash<<5)+hash)+c;
	}
	return hash;
}

int leggiI(char *s)
{
	char c;
	int sum=0;
	c=getchar_unlocked();
	while(c >= '0' && c <= '9')
	{
		sum = (sum<<3)+(sum<<1)+(c-48);
		c = getchar_unlocked();
	}
	*s = c; //restituisce il carattere per cui è uscito (o lo spazio o lo /n o un carattere intero)
	return sum; //restituisce la somma
}

char* leggiS(char *s)
{
	char c;
	int i=0;
	c=getchar_unlocked();
	while((c >= '0' && c <= '9') || (c >= 'A' && c <= 'Z') || (c >= 'a' && c <= 'z') || c == '_') //esce solo con lo spazio e con \n
	{
		stringa[i] = c;
		c = getchar_unlocked();
		i++;
	}
	*s=c; //restituisco o \n o lo spazio
	stringa[i] = '\0'; //carattere terminale della stringa
	return stringa; //ritorna la parla letta
}

//funzioni lista ingredienti --------------------

void inserisciI(t_listaingredienti* l, char *s, int q, unsigned long cod) //inserisce un nuovo ingrediente alla lista in ordine alfabetico
{
	t_listaingredienti punt, puntcor = *l, puntprec = NULL;
	while (puntcor != NULL && cod > puntcor->ingrediente.cod)
	{
		puntprec = puntcor;
		puntcor = puntcor->next;
	}
	punt = (t_listaingredienti)malloc(sizeof(I_nodo));
	strcpy(punt->ingrediente.nome, s);
	punt->ingrediente.quantita = q;
	punt->ingrediente.cod = cod;
	punt->next = puntcor;
	if (puntprec != NULL)
	{
		puntprec->next = punt;
	}
	else
	{
		*l = punt;
	}
}

//funzione lista ricettario --------------------

void inserisciR(t_ricetta ric)
{
    //long pos = ric.cod % len;
    ricet ricetta=(ricet)malloc(sizeof(t_ricetta));
    ricetta->cod=ric.cod;
    strcpy(ricetta->nome, ric.nome);
    ricetta->peso=ric.peso;
    ricetta->listaingredienti=ric.listaingredienti;
    printf("aggiunta\n");
    if(ricettario[ric.cod % len] != NULL)//se c'è già un ingrediente
    {
        ricetta->next = ricettario[ric.cod % len];
        ricettario[ric.cod % len]=ricetta;
        return;
    }
    ricetta->next=NULL;
    ricettario[ric.cod % len]=ricetta;

    return;
}

void cancunoR(char *s, unsigned long cod)
{
    //long pos=cod % len;
    ricet prec = NULL, cur = ricettario[cod % len];
    while(cod != cur->cod || strcmp(s, cur->nome) != 0)
    {
        prec=cur;
        cur=cur->next;
    }
    if(prec == NULL) //è il primo elemento
    {
        ricettario[cod % len]=cur->next;
        cancellalistaI(cur->listaingredienti);
        free(cur);
        return;
    }
    prec->next=cur->next;
    cancellalistaI(cur->listaingredienti);
    free(cur);
    return;
}

ricet controlloR(char *s, unsigned long cod)
{
    //long pos = cod % len;
    ricet cur = ricettario[cod % len];
    while (cur != NULL)
    {
        if(cur->cod == cod)
        {
            if(strcmp(cur->nome, s) == 0)
            {
                return cur;
            }
        }
        cur=cur->next;
    }
    return NULL;
}

//funzione lista ordini --------------------

mag controlloM(char *s, unsigned long cod) //ricerca un nodo, restituisce il rispettivo
{
    //long pos = cod % len;
    mag cur = magazzino[cod % len];
    while(cur != NULL)
    {
        if(cod == cur->cod)
        {
            if(strcmp(s, cur->ingrediente) == 0)
            {
               return cur;
            }
        }
       cur = cur->next;
    }
    return NULL;
}

t_lotti inserisciL(t_lotti lotto, int q, int scad)
{
	t_lotti cur=lotto, prec=NULL;
	while(cur!=NULL && cur->scadenza<scad)
	{
		prec=cur;
		cur=cur->next;
	}
	if(cur!=NULL&&cur->scadenza==scad)//se la scadenza gi� c'� aumento le quantit�
	{
		cur->q+=q;
		return lotto;
	}
    t_lotti punt=(t_lotti)malloc(sizeof(L_nodo));
	punt->q=q;
	punt->scadenza=scad;
	punt->next=cur;
	if(prec!=NULL)
	{
		prec->next=punt;
	}
	else
	{
		return punt;
	}
	return lotto;
}

void inserisciM(char* nome, int q, int scad, unsigned long cod)
{
    //long pos = cod % len;
    if(magazzino[cod % len] == NULL) //la cella è vuota
    {
        mag punt = (mag)malloc(sizeof(t_magazzino));
        punt->cod=cod;
        strcpy(punt->ingrediente, nome);
        punt->peso=q;
        punt->lotto=NULL;
        punt->lotto=inserisciL(punt->lotto, q, scad);
        punt->next=NULL;
        magazzino[cod % len]=punt;
        return;
    }
    //se c'è qualcosa
    mag punt=controlloM(nome, cod);
    if(punt != NULL) //l'ingrediente c'è
    {
        punt->lotto=inserisciL(punt->lotto, q, scad);//inserisco il lotto
        punt->peso += q; //aumento il peso del lotto
        return;
    }
    //se non c'è lo inserisco come primo elemento
    punt = (mag)malloc(sizeof(t_magazzino));
    punt->cod=cod;
    strcpy(punt->ingrediente, nome);
    punt->peso=q;
    punt->lotto=NULL;
    punt->lotto=inserisciL(punt->lotto, q, scad);
    punt->next=magazzino[cod % len];
    magazzino[cod % len]=punt;
    return;
}

t_lotti cancellaL(t_lotti lotto) //elimino il primo ingrediente
{
	t_lotti cur=lotto->next;
	free(lotto);
	return cur;
}

void cancunoM(mag nodo) //elimina un nodo del magazzino (devo passargli il nodo)
{
	//long pos = nodo->cod % len;
    mag prec = NULL, cur = magazzino[nodo->cod % len];
    while(cur != nodo)
    {
        prec = cur;
        cur = cur->next;
    }
    if(prec == NULL)//era il primo ingrediente
    {
        magazzino[nodo->cod % len]=magazzino[nodo->cod % len]->next;
        free(nodo);
        return;
    }
    prec->next = cur->next;
    free(nodo); // non elimino i lotti in quanto devono essere già eliminati prima
    return;
}

void faiord(t_listaingredienti ingredienti, int q, mag* tot)
{
	int prec, res, i=0;
	while(ingredienti!=NULL)
	{
		tot[i]->peso-=(ingredienti->ingrediente.quantita*q);
		if(tot[i]->peso<0)//non avrei potuto fare l'ordine
		{
			return;
		}
		else if(tot[i]->peso==0)//ho finito l'ingrediente nella dispensa, libero tutti i lotti e il nodo
		{
			cancellalistaL(tot[i]->lotto);
			cancunoM(tot[i]);//elimino il nodo
		}
		else
		{
			//se sono qui avanza qualche lotto
			res = (ingredienti->ingrediente.quantita*q);
			while(res > 0)
			{	
				prec=res;
				res -= tot[i]->lotto->q;
				if(res >= 0)//devo eliminare quel lotto
				{
					tot[i]->lotto=cancellaL(tot[i]->lotto);
				}
				else
				{
					tot[i]->lotto->q -= prec; //diminuisco la quantit�
				}
			}
		}
		ingredienti=ingredienti->next;
		i++;
	}
	//elimina ingredienti uno per uno, se elimini intera lista cancella nodo e ripara. 
	//ogni ingrediente riparti da capo
	//trova un modo per salvare bene quantit� su un lotto dopo che togli un ingrediente(quando la quantit� del lotto avanza)
}

int check(t_ordine ordine)//restituisce l'1 se ho fatto l'ordine, 0 altrimetni
{
	t_listaingredienti ingredienti = NULL;

	//1- vedo tutti gli ingredienti necessari per fare l'ordine

	ingredienti=ordine.ricetta->listaingredienti;
	
	//2- vedo se tutti questi ingredienti ci sono nel magazzino(fai funzione a parte)
	
	t_listaingredienti ingr = ingredienti;
	mag cur; 
	int i=0;
	while(ingr!=NULL)
	{
		cur = controlloM(ingr->ingrediente.nome, ingr->ingrediente.cod);
		if(cur == NULL || cur->peso < (ingr->ingrediente.quantita * ordine.q)) //se non lo posso fare return 0
		{
			return 0;
		}
		tot[i]=cur;
		i++;
		ingr = ingr->next;
	}
	
	//se esco da qui posso farlo
	//3- faccio l'ordine ed elimino i lotti/le quantit� dal magazzino(fai funzione che restituisce la lista, in modo che eguagli e salva il valore della nuova quantit�
	//per ogni ingrediente riparte da capo, quindi se lista � null chiama subito il ripara

	faiord(ingredienti, ordine.q, tot);
	return 1;
}

t_listaordini inseriscifO(t_listaordini lista, t_ordine a, int t, t_listaordini* coda)
{
	t_listaordini punt, cur=*coda;
	punt = (t_listaordini)malloc(sizeof(A_nodo));
	punt->ordine = a;
	punt->t = t;
	punt->next = NULL;
	if (lista == NULL)
	{
        *coda=punt;
		return punt;
	}
	else
	{
		cur->next=punt;
        *coda=punt;
		return lista;
	}
}

void inserisciOnm(t_listaordini* l, t_listaordini nuovo, t_listaordini *coda) //inserisce un nuovo ordine alla lista in base al tempo di arrivo(cronologico)
{//senzamalloc
	t_listaordini puntcor, puntprec = NULL;
	puntcor = *l;
	while (puntcor != NULL && nuovo->t >= puntcor->t)
	{
		puntprec = puntcor;
		puntcor = puntcor->next;
	}
	nuovo->next = puntcor;
	if(puntcor == NULL)
	{
		*coda=nuovo;
	}
	if (puntprec != NULL)
	{
		puntprec->next = nuovo;
	}
	else
	{
		*l = nuovo;
	}
}

t_listaordini cancattesa(t_listaordini lista, t_listaordini* sped, t_listaordini *codaa, t_listaordini *codas) //elimina gli ordini che si possono fare dopo un rifornimento, li mette in lista di spedizione
{
	t_listaordini temp, prec=NULL, cur=lista;
	while (cur != NULL)
	{
		if (check(cur->ordine) == 1)//se posso farlo
		{
			temp = cur->next;
			inserisciOnm(sped, cur, codas); //lo metto in spedizione in ordine cronologico
			if (prec == NULL)//se era il primo elemento
			{
				lista = temp;
			}
			else
			{
				prec->next = temp;//stacco il puntatore
			}
			cur = temp;//passo al nodo successivo
		}
		else
		{
            *codaa=cur;
			prec = cur;//passo al successivo
			cur = cur->next;
		}
	}
	return lista;
}

void stampaordine(t_ordine ordine)
{
	printf("%s %d \n", ordine.ricetta->nome, ordine.q);
}

void inserisciOP(t_listaordini* l, t_listaordini nuovo) //inserisce un nuovo ordine alla lista in base al peso
{
	t_listaordini puntcor, puntprec = NULL;
	puntcor = *l;
	while (puntcor != NULL && nuovo->ordine.peso <= puntcor->ordine.peso)
	{
		puntprec = puntcor;
		puntcor = puntcor->next;
	}
	nuovo->next = puntcor;
	if (puntprec != NULL)
	{
		puntprec->next = nuovo;
	}
	else
	{
		*l = nuovo;
	}
}

t_listaordini cancsped(t_listaordini lista, int q) //cancella ordini che entrano nel camioncino(funziona bene)
{
	t_listaordini puntTemp = lista, stampa = NULL, t;
	int c = 0;
	if (lista != NULL)
	{
		while (puntTemp != NULL && q > 0)
		{
			if (q - puntTemp->ordine.peso >= 0)//lo elimino
			{
				c = 1;
				q -= puntTemp->ordine.peso;
				lista = puntTemp->next;
				inserisciOP(&stampa, puntTemp);
				puntTemp = lista;
			}
			else
			{
				q = 0;
				if (c == 0)
				{
					printf("camioncino vuoto\n");
					return puntTemp;//non elimino stampa poich� =NULL se entro qui
				}
			}
		}
	}
	else
	{
		printf("camioncino vuoto\n");
		return NULL;//non elimino stampa poich� =NULL se entro qui
	}

	//adesso stampa contiene gli ordini da spedire in ordine di peso, li devo visualizzare a schermo

	while (stampa != NULL)
	{
		printf("%d ", stampa->t);
		stampaordine(stampa->ordine);
		t = stampa->next;
		free(stampa); //cancello stampa, non la user� pi�
		stampa = t;
	}
	return puntTemp; //ritorno la lista senza ordini spediti
}

int controllaricetta(t_listaordini lista, char* s, unsigned long cod)//1 se non ci sono ordini con quella ricetta
{//scorre gli ordini
	if (lista == NULL)
	{
		return 1;
	}
	if (lista->ordine.ricetta->cod==cod && strcmp(lista->ordine.ricetta->nome, s) == 0)
	{
		return 0;
	}
	return controllaricetta(lista->next, s, cod);
}

void scadenze(int t) //controlla i prodotti scaduti e li elimina
{
    mag cur, temp;
    int i;
    for(i=0; i < len; i++)
    {
        if(magazzino[i] != NULL)
        {
            cur = magazzino[i];
            while(cur != NULL)
            {
				temp=cur->next;
                while(cur->lotto->scadenza <= t)
                {
					cur->peso -= cur->lotto->q;
                    cur->lotto = cancellaL(cur->lotto);
                    if(cur->lotto == NULL)
                    {
                        cancunoM(cur);
						break;
                    }
                }
                cur=temp;
            }
        }
    }
}

//main --------------------

int main()
{
	char c, parola[30];
	int periodicita, dimensione, dato, scad, t = 0;
    unsigned long cod;
	t_listaordini attesa = NULL, spedizione = NULL, a_tail=NULL, s_tail=NULL;
	t_ordine ordine;
	t_ricetta ricetta;
    ricetta.next=NULL;
	ricetta.listaingredienti = NULL;
	periodicita = leggiI(&c); // c ha lo spazio
	dimensione = leggiI(&c); // c ha lo \n
	strcpy(parola, leggiS(&c));
	while (strlen(parola) != 0)
	{
		//printf("\n%d, %s --- ", t, parola);
		if (t != 0 && t % periodicita == 0)//faccio le spedizioni
		{
			spedizione = cancsped(spedizione, dimensione);//spedisce gli ordini
		}
		if (strncmp(parola, "aggiungi_ricetta", 1) == 0)
		{
			strcpy(ricetta.nome, leggiS(&c));
            ricetta.cod=converti(ricetta.nome);
			dato=0;
			if (controlloR(ricetta.nome, ricetta.cod) == NULL)//se la ricetta non c'�
			{
				while (c != '\n')//va avanti fino a che non si va a capo
				{
					strcpy(parola, leggiS(&c)); 
					scad = leggiI(&c); //se c ha lo spazio continua, se ha \n no
                    cod=converti(parola);
					inserisciI(&ricetta.listaingredienti, parola, scad, cod);
					dato += scad;
				}
				ricetta.peso=dato;
				inserisciR(ricetta);
				ricetta.listaingredienti = NULL;//non devo cancellare nulla poich� inserisco direttamente nel ricettario
			}
			else
			{
				while (getchar() != '\n')
				{
					//scorre senza salvare nulla	
				}
				printf("ignorato\n");
			}
		}
		else if (strncmp(parola, "rimuovi_ricetta", 3) == 0)
		{
			strcpy(parola, leggiS(&c));//nome ricetta da eliminare
            cod=converti(parola);
			if (controlloR(parola, cod) == NULL)//non modifica ricettario il controlloR
			{
				printf("non presente\n");
			}
			else//se c'� devo vedere se ci sono ordini in sospeso
			{
				if (controllaricetta(attesa, parola, cod) == 1 && controllaricetta(spedizione, parola, cod) == 1)//non ci sono ordini
				{
					cancunoR(controlloR(parola, cod)->nome, cod);
					printf("rimossa\n");
				}
				else
				{
					printf("ordini in sospeso\n");
				}
			}
		}
		else if (strncmp(parola, "rifornimento", 3) == 0)
		{
			while (c != '\n')//va avanti fino a che non si va a capo
			{
				strcpy(parola, leggiS(&c));
				dato = leggiI(&c);
				scad = leggiI(&c); //c = spazio o a \n, in base a quando finisce
                cod=converti(parola);
				if(scad > t)
				{
					inserisciM(parola, dato, scad, cod);
				}
			}
			scadenze(t);//elimino gli elementi scaduti
			attesa = cancattesa(attesa, &spedizione, &a_tail, &s_tail);
			printf("rifornito\n");
		}
		else
		{
			strcpy(parola, leggiS(&c));
			ordine.q = leggiI(&c);
            cod=converti(parola);
			ordine.ricetta=controlloR(parola, cod);
			if (ordine.ricetta == NULL) //se la ricetta non c'� rifiuto l'ordine
			{				
				printf("rifiutato\n");
			}
			else
			{
				ordine.peso=ordine.q*ordine.ricetta->peso;
				printf("accettato\n");
				if (check(ordine) == 1) //se lo posso fare lo metto in spedizioni
				{
					spedizione = inseriscifO(spedizione, ordine, t, &s_tail);
				}
				else//altrimenti lo metto in attesa
				{
					attesa = inseriscifO(attesa, ordine, t, &a_tail);
				}
			}
		}
		t++;
		strcpy(parola, leggiS(&c));
	}

	//gestisco l'ultima riga senza comandi

	if (t % periodicita == 0)
	{
		spedizione = cancsped(spedizione, dimensione);
	}

	//elimino le liste, le funzioni fanno la free anche delle sottoliste
	
	return 0;
}
