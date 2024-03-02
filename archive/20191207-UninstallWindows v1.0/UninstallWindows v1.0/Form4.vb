Public Class Form4
    Private Sub Form4_Load(sender As Object, e As EventArgs) Handles MyBase.Load

    End Sub
    Private Sub Button1_Click(ByVal sender As System.Object, ByVal e As System.EventArgs) Handles Button1.Click
        Dim oForm As New Form5()

        Me.Hide()
        oForm.ShowDialog()

        Me.Close()
    End Sub
End Class