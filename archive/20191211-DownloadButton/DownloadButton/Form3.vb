Public Class Form3
    Public Property StringPass As String
    Public Property BtnCol As String
    Public Property BtnHov As String
    Public Property BtnTxt As String
    Private Sub LinkLabel1_LinkClicked(sender As Object, e As LinkLabelLinkClickedEventArgs) Handles LinkLabel1.LinkClicked
        Process.Start(StringPass)
    End Sub

    Private Sub Button4_Click(sender As Object, e As EventArgs) Handles Button4.Click
        If TextBox2.Text = ("") Then
            MsgBox("No input link detected. Please input some.", vbOKOnly + vbExclamation)
            TextBox2.Enabled = False
            Button4.Enabled = False
        Else
            Clipboard.SetText(TextBox2.Text)
            MsgBox("Code copied to clipboard.", vbOKOnly + vbInformation)
        End If
    End Sub

    Private Sub Button5_Click(sender As Object, e As EventArgs) Handles Button5.Click
        Dim back As New Form2()
        back.Show()
        Me.Hide()
    End Sub

    Private Sub Button1_Click(sender As Object, e As EventArgs) Handles Button1.Click
        Me.Close()
    End Sub

    Private Sub Button2_Click(sender As Object, e As EventArgs) Handles Button2.Click
        If TextBox1.Text = ("") Then
            MsgBox("No input link detected. Please input some.", vbOKOnly + vbExclamation)
            TextBox1.Enabled = False
            Button2.Enabled = False
        Else
            Clipboard.SetText(TextBox1.Text)
            MsgBox("Code copied to clipboard.", vbOKOnly + vbInformation)
        End If
    End Sub

    Private Sub Button3_Click(sender As Object, e As EventArgs) Handles Button3.Click
        Const quote = """"
        Dim BtnCol As String = (ComboBox1.Text)
        Dim BtnHov As String = (ComboBox2.Text)
        Dim BtnTxt As String = (TextBox3.Text)
        TextBox1.Text = ("<!DOCTYPE html>
<html>
<head>
<meta name=" + quote + "viewport" + quote + " content=" + quote + "width=device-width, initial-scale=1" + quote + ">
<!-- Add icon library -->
<link rel=" + quote + "stylesheet" + quote + " href=" + quote + TextBox2.Text + quote + ">
<style>
.btn {
  background-color: " + BtnCol + ";
  border: none;
  color: white;
  padding: 12px 30px;
  cursor: pointer;
  font-size: 20px;
}

.btn:hover {
  background-color: " + BtnHov + ";
}
</style>
</head>
<body>

<button class=" + quote + "btn" + quote + "><i class=" + quote + "fa fa-download" + quote + "></i> " + BtnTxt + "</button>

</body>
</html>
")
    End Sub
End Class