#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include <time.h>
#include <unistd.h>
#include <signal.h>
#include <sys/wait.h>
void handler(int signum)
{
   sleep(1);
   printf("je suis le pere du pid %d je suis reveille par mon fils a l'aide du signal %d",getpid(),signum);
   printf("\nFin du jeu :D\n");
   exit(0);
}
// fonction pour recuperer un combre aleatoire.
int nombreAleatoire(int nombreMax)
{
    srand(time(NULL));
    return (rand() % nombreMax);
}

int getHiddenWord(char *Hidden_Word)
{
    FILE* fichier = NULL; // Le pointeur de fichier qui va contenir notre fichier
    int nombreMots = 0, numMotChoisi = 0, i = 0;
    int caractereLu = 0;
    fichier = fopen("listWords.txt", "r"); // On ouvre le fichier en lecture seule
    // On v�rifie si on a r�ussi � ouvrir le fichier
    if (fichier == NULL) // Si on n'a PAS r�ussi � ouvrir le fichier
    {
        printf("\nFichier introuvable!");
        return 0; // On retourne 0 pour indiquer que la fonction a �chou�
    }

    // On compte le nombre de mots dans le fichier (il suffit de compter les caractere de chaque ligne.
    do
    {
        caractereLu = fgetc(fichier);
        if (caractereLu == '\n')
            nombreMots++;
    } while(caractereLu != EOF);

    numMotChoisi = nombreAleatoire(nombreMots); // On prend une ligne aleatoire

    // On recommence � lire le fichier depuis le d�but. On s'arr�te lorsqu'on est arriv�s au bon mot
    rewind(fichier);
    while (numMotChoisi > 0)
    {
        caractereLu = fgetc(fichier);
        if (caractereLu == '\n')
            numMotChoisi--;
    }

   // Le curseur du fichier est positionn� au bon endroit.
   // On n'a plus qu'� faire un fgets qui lira la ligne 
    fgets(Hidden_Word, 100, fichier);

    Hidden_Word[strlen(Hidden_Word) - 1] = '\0';
    fclose(fichier);

    return 1; 
}


char lireCaractere()
{
    char caractere = 0;

    caractere = getchar(); // On lit le premier caract�re

    // On lit les autres caract�res m�moris�s un � un jusqu'� l'\n
    while (getchar() != '\n') ;

    return caractere; // On retourne le premier caract�re qu'on a lu
}


int WinTest(int ListCharacter [], int tailleMot)
{
    int i = 0;
    int correct = 1;

    for (i = 0 ; i < tailleMot ; i++)
    {
        if (ListCharacter [i] == 0)
            correct = 0;
    }

    return correct;
}

int FindLetter(char lettre, char Hidden_Word[], int ListCharacter[])
{
    int i = 0;
    int vrai = 0;

    // On parcourt Hidden_Word pour v�rifier si la lettre propos�e y est
    for (i = 0 ; Hidden_Word[i] != '\0' ; i++)
    {
        if (lettre == Hidden_Word[i]) // Si la lettre y est
        {
            vrai = 1; // On m�morise que c'�tait une bonne lettre
            ListCharacter[i] = 1; // On met � 1 le case du tableau de bool�ens correspondant � la lettre actuelle        
    	}
    }
    return vrai;
}
int LoseTest(int Nbr_vie, char Hidden_Word[])
{
    if(Nbr_vie==0)
    {
	printf("\nVous avez perdu!\n");
        printf("Vous n'avez pas deviner le mot : %s\n", Hidden_Word);
    }
}
int DisplayWord(int tailleMot, char Hidden_Word[], int ListCharacter[])
{
    int i=0;
    printf("\nDeviner le mot : ");
    /* On affiche le HiddenWord en masquant les lettres non trouv�es*/
    for (i = 0 ; i < tailleMot ; i++)
    {
        if (ListCharacter[i]) // Si on a trouv� la lettre
             printf("%c", Hidden_Word[i]); // On l'affiche
        else
            printf("-"); // Sinon, on affiche tiret pour les lettres non trouv�es
    }
}
int main()
{
    pid_t pid;
    int tailleMot;
    char lettre = 0; // Stocke la lettre propos�e par l'utilisateur
    char Hidden_Word[100] = {0}; // Ce sera le mot � trouver
    int *ListCharacter ; // Un tableau de bool�ens. Chaque case correspond � une lettre du mot secret. 0 = lettre non trouv�e, 1 = lettre trouv�e
    int Nbr_vie = 5; // Compteur de coups restants (0 = mort)
    int i = 0; // Une petite variable pour parcourir les tableaux
    int lire1,lire2,ecrire1,ecrire2;
    int p2[2];
    pipe(p2);
    int pid1=getpid();
    if((pid=fork())!=0)
    {
	int pid1=getpid();
	getHiddenWord(Hidden_Word);
        tailleMot = strlen(Hidden_Word);
        ListCharacter = malloc(tailleMot * sizeof(int)); // On alloue dynamiquement le tableau 
        printf("je suis le pere mon pid est %d \n",pid1);
	printf("----- Bienvenu au jeu de mots -----\n");
        for (i = 0 ; i < tailleMot ; i++)
        {
             ListCharacter[i] = 0;
        }
	close(p2[0]);
	ecrire1= write(p2[1],&tailleMot,1*sizeof(int));   //l'envoi de la taille du mot au fils 
	ecrire2= write(p2[1],Hidden_Word,tailleMot*sizeof(char)); // l'envoi du Hidden_Word au fils
	printf("je donne la main a mon fils %d pour lancer le jeu\n",pid);
	signal(SIGINT,handler);
	close(p2[1]);        
	pause();
    }
    else// je suis chez le fils
    {
	Nbr_vie=5;
	close(p2[1]);
	lire1=read(p2[0],&tailleMot,1*sizeof(int)); // lecture de la taille du mot
	lire2= read(p2[0],Hidden_Word,tailleMot*sizeof(char)); // lecture de hidden_word
        ListCharacter = malloc(tailleMot * sizeof(int)); // On alloue dynamiquement le tableau 
        printf("je suis le fils mon pid est %d",getpid());    
	// On continue � jouer tant qu'il reste au moins un coup � jouer ou qu'on n'a pas gagn�
         while (Nbr_vie !=0 && !WinTest(ListCharacter , tailleMot))
        {
	    DisplayWord(tailleMot,Hidden_Word,ListCharacter);
            printf("\n\nSaisir un caractere : ");
            lettre = lireCaractere();

            // Si ce n'�tait PAS la bonne lettre
            if (!FindLetter(lettre, Hidden_Word, ListCharacter ))
            {
		        printf("Incorrect!\n");
                Nbr_vie--; // decrementation de nombre de vie
                printf("Tentative restant: %d", Nbr_vie);
            }
            else
            {
                printf("Correct");
            }
	    LoseTest(Nbr_vie,Hidden_Word);
        }
        if (WinTest(ListCharacter , tailleMot))
        {
            printf("\nVous avez deviner le mot : %s\n", Hidden_Word);
            printf("Bravo Vous avez gagner :D\n");
	}	
	kill(pid1,SIGINT);
	wait(0);
    }
    return 0;
}
