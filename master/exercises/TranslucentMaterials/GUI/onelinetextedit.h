#ifndef ONELINETEXTEDIT_H
#define ONELINETEXTEDIT_H
#include <QPlainTextEdit>

class OneLineTextEdit : public QPlainTextEdit
{
    Q_OBJECT
public:
    explicit OneLineTextEdit(QWidget *parent = 0);

    QSize sizeHint() const;

signals:
    void onTextChanged();

protected:
    virtual void keyPressEvent(QKeyEvent *e);
    virtual void focusOutEvent(QFocusEvent *event);
};

#endif // ONELINETEXTEDIT_H
