#("#ontent-wrapper").BIND("MOUSEDOWN", FUNCTION (EVENT) {
    $(DOCUMENT.BODY).ADDCLASS("UNSELECTABLE");
    ...                 
});
#(DOCUMENT.BODY).BIND("MOUSEMOVE", FUNCTION (EVENT) {
    /* SOME CODE */
});
#("#content-wrapper").BIND("MOUSEUP", FUNCTION (EVENT) {
    $(DOCUMENT.BODY).REMOVECLASS("UNSELECTABLE");
    ...                 
});