Public Class Form1
    Private Sub Button4_Click(sender As Object, e As EventArgs) Handles Button4.Click
        Me.Close()
    End Sub

    Private Sub Button3_Click(sender As Object, e As EventArgs) Handles Button3.Click
        MsgBox("(c) 2018-2019 null8626 Programming - DownloadButton.exe is a program that may help people who need to have help creating downloadable links/buttons to their web.

VERSION 1.0 - December 11, 2019

This program is NOT a malware. Even IF it is, it may be caused by an external program, not by the programmer.", vbOKOnly + vbInformation, "About DownloadButton.exe")
    End Sub

    Private Sub Button1_Click(sender As Object, e As EventArgs) Handles Button1.Click
        Dim box = New Form2()
        box.Show()
        Me.Hide()
    End Sub

    Private Sub Button2_Click(sender As Object, e As EventArgs) Handles Button2.Click
        Const quote = """"
        MsgBox("To create a button (with a link), use this code.
<button onclick=" + quote + "window.location.href = 'YOUR LINK HERE';" + quote + ">YOUR TEXT HERE</button>", vbOKOnly + vbInformation)
    End Sub
End Class
