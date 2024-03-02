Public Class Form2
    Dim BtnCol As String = ("Blue")
    Dim btnhov As string = ("Red")
    Dim btntxt As String = ("Download here")

    Private Sub Button3_Click(sender As Object, e As EventArgs) Handles Button3.Click
        If TextBox1.Text = ("") Then
            MsgBox("No input link detected. Please input some.", vbOKOnly + vbExclamation)
        Else
            Dim str1 As String = TextBox1.Text
            str1 = str1.Remove(0, 32)
            str1 = str1.Remove(str1.Length - 5)
            Dim str2 As String = ("https://drive.google.com/uc?export=download&id=" + str1)
            TextBox2.Enabled = True
            Button1.Enabled = True
            Button2.Enabled = True
            Button4.Enabled = True
            Button5.Enabled = True
            TextBox2.Text = str2
        End If
    End Sub

    Private Sub Button1_Click(sender As Object, e As EventArgs) Handles Button1.Click
        If TextBox2.Text = ("") Then
            MsgBox("No input link detected. Please input some.", vbOKOnly + vbExclamation)
            TextBox2.Enabled = False
            Button1.Enabled = False
            Button2.Enabled = False
            Button4.Enabled = False
            Button5.Enabled = False
        Else
            Process.Start(TextBox2.Text)
        End If
    End Sub

    Private Sub Button2_Click(sender As Object, e As EventArgs) Handles Button2.Click
        If TextBox2.Text = ("") Then
            MsgBox("No input link detected. Please input some.", vbOKOnly + vbExclamation)
            TextBox2.Enabled = False
            Button1.Enabled = False
            Button2.Enabled = False
            Button4.Enabled = False
            Button5.Enabled = False
        Else
            Clipboard.SetText(TextBox2.Text)
            MsgBox("Link: " + TextBox2.Text + " Has copied to clipboard.", vbOKOnly + vbInformation)
        End If
    End Sub

    Private Sub Button5_Click(sender As Object, e As EventArgs) Handles Button5.Click
        TextBox2.Text = ("")
        TextBox1.Text = ("")
        TextBox2.Enabled = False
        Button1.Enabled = False
        Button2.Enabled = False
        Button4.Enabled = False
        Button5.Enabled = False
    End Sub

    Private Sub Button4_Click(sender As Object, e As EventArgs) Handles Button4.Click
        If TextBox2.Text = ("") Then
            MsgBox("No input link detected. Please input some.", vbOKOnly + vbExclamation)
            TextBox2.Enabled = False
            Button1.Enabled = False
            Button2.Enabled = False
            Button4.Enabled = False
            Button5.Enabled = False
        Else
            Dim passtr As New Form3
            Const quote = """"
            passtr.StringPass = TextBox2.Text
            passtr.LinkLabel1.Text = TextBox2.Text
            passtr.TextBox1.Text = "<!DOCTYPE html>
<html>
<head>
<meta name=" + quote + "viewport" + quote + " content=" + quote + "width=device-width, initial-scale=1" + quote + ">
<!-- Add icon library -->
<link rel=" + quote + "stylesheet" + quote + " href=" + quote + TextBox2.Text + quote + ">
<style>
.btn {
  background-color: Blue;
  border: none;
  color: white;
  padding: 12px 30px;
  cursor: pointer;
  font-size: 20px;
}

.btn:hover {
  background-color: Red;
}
</style>
</head>
<body>

<button class=" + quote + "btn" + quote + "><i class=" + quote + "fa fa-download" + quote + "></i> Download here...</button>

</body>
</html>
"
            passtr.TextBox2.Text = "<button onclick=" + quote + "window.location.href = '" + TextBox2.Text + "';" + quote + ">" + BtnTxt + "</button>"
            passtr.Show()
            Me.Hide()
        End If
    End Sub
End Class