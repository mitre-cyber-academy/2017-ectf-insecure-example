
// // #pragma startup e_main

// int e_main() {

//     int pid = fork();

//     if (pid == 0) {
//         main();
//     } else {
//         int status;
//         while(wait() > 0) { /* no-op */ ; }
//         sleep(1);
//         main();
//     }
// }