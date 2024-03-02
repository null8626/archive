Public Class Form3
    Private Sub LinkLabel1_LinkClicked(ByVal sender As System.Object, ByVal e As System.Windows.Forms.LinkLabelLinkClickedEventArgs)
        System.Diagnostics.Process.Start("http://giphy.com/gifs/reaction-9o9dh1JRGThC1qxGTJ")
    End Sub
    Private Sub Button1_Click(ByVal sender As System.Object, ByVal e As System.EventArgs) Handles Button1.Click
        Dim oForm As New Form2()

        Me.Hide()
        oForm.ShowDialog()

        Me.Close()
    End Sub
End Class