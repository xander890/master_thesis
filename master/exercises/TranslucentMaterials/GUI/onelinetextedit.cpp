#include "onelinetextedit.h"

#include <QKeyEvent>


OneLineTextEdit::OneLineTextEdit(QWidget *parent) :
    QPlainTextEdit(parent)
{
    setWordWrapMode (QTextOption::NoWrap);
    setHorizontalScrollBarPolicy (Qt::ScrollBarAlwaysOff);
    setVerticalScrollBarPolicy (Qt::ScrollBarAlwaysOff);
    setTabChangesFocus (true);
    setSizePolicy (QSizePolicy::Expanding, QSizePolicy::Fixed);
}

QSize OneLineTextEdit::sizeHint () const
{
    QFontMetrics fm(font());
    int h = fm.height();
    int w = fm.width(QLatin1Char('x'));

    return QSize(w, h);
}

void OneLineTextEdit::focusOutEvent(QFocusEvent *event)
{
    emit onTextChanged();
    QPlainTextEdit::focusOutEvent(event);
}

void OneLineTextEdit::keyPressEvent (QKeyEvent *e)
{
    if ((e->key () == Qt::Key_Enter) || (e->key () == Qt::Key_Return)){
        emit onTextChanged();
        e->ignore ();
    }else{
        QPlainTextEdit::keyPressEvent (e);
    }
}
